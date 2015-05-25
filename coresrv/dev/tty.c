#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
//#include <pthread.h>

#include <sys/keymap.h>

#include <kernel/kernel.h>
#include <kernel/io.h>
#include <kernel/vga.h>

#include <coresrv/kbd.h>
#include <coresrv/tty.h>
#include <coresrv/dev.h>

#define N_TTYS 3
#define TTY_HEIGHT 100

#define MEMORY_SIZE (VGA_WIDTH*TTY_HEIGHT)
#define WINDOW_SIZE (VGA_WIDTH*VGA_HEIGHT)

struct dev_tty_buffer {
  unsigned id;
  uint16_t buffer[VGA_WIDTH*TTY_HEIGHT];
  unsigned cursor_index;
  unsigned window_end;
  unsigned screen_end;
  uint16_t color;
};

struct dev_tty {
  struct dev_tty_buffer ttys[N_TTYS];
  volatile unsigned current_tty;
  //pthread_cond_t cond; // syncs access to current_tty.
  //pthread_mutex_t mutex;
  
  uint16_t vga_shadow[WINDOW_SIZE];
  unsigned vga_cursor_x;
  unsigned vga_cursor_y;
  unsigned cursor_visible;
  unsigned vga_copied;
  unsigned menu_visible;
  uint16_t menusave[VGA_WIDTH];
} dev_tty;

extern uint16_t vga_buffer[];

static void update_vga_cursor(struct dev_tty_buffer *tty) {
  if (tty->id != dev_tty.current_tty) return;

  unsigned screen_begin = tty->screen_end - WINDOW_SIZE;
  unsigned position = tty->cursor_index - screen_begin;
  if (tty->cursor_index < screen_begin || position >= WINDOW_SIZE) {
    // Hide cursor
    position = 0xffff;
  }

  // cursor LOW port to vga INDEX register
  outb(0x3D4, 0x0F);
  outb(0x3D5, (unsigned char)(position&0xFF));
  // cursor HIGH port to vga INDEX register
  outb(0x3D4, 0x0E);
  outb(0x3D5, (unsigned char )((position>>8)&0xFF));
}

static void update_vga_part(struct dev_tty_buffer *tty,
                            unsigned index, size_t size) {
  if (tty->id != dev_tty.current_tty) return;

  unsigned screen_begin = tty->screen_end - WINDOW_SIZE;
  for (; size; --size, ++index) {
    if (index >= screen_begin && index < tty->screen_end) {
      unsigned vga_index = index - screen_begin;
      uint16_t d = tty->buffer[index % MEMORY_SIZE];
      if (d != dev_tty.vga_shadow[vga_index]) {
        dev_tty.vga_shadow[vga_index] = d;
        vga_buffer[vga_index] = d;
      }
    }
  }
  update_vga_cursor(tty);
}

static void update_vga_full(struct dev_tty_buffer *tty) {
  if (tty->id != dev_tty.current_tty) return;

  unsigned index = tty->screen_end - WINDOW_SIZE;
  for (unsigned i = 0; i < WINDOW_SIZE; ++i, ++index) {
    uint16_t d = tty->buffer[index % MEMORY_SIZE];
    if (dev_tty.menu_visible && i < 80) {
      dev_tty.menusave[i] = d;
    } else if (d != dev_tty.vga_shadow[i]) {
      dev_tty.vga_shadow[i] = d;
      vga_buffer[i] = d;
    }
  }
  update_vga_cursor(tty);

  if (dev_tty.menu_visible) {
    uint16_t color = make_vgaentry(0, make_color(COLOR_WHITE,
                                                 COLOR_BLUE));
    char menu[81] = ("  1  2  3                               "
                     "         16M/133M  L:1.3/0.7/0.5  xx:xx ");
    // draw current time:
    time_t rawtime = time(NULL);
    struct tm* ltime = localtime(&rawtime);
    //TODO: use %02i when printf supports it.
    sprintf(menu+74, "%i:%i", ltime->tm_hour, ltime->tm_min);
    // draw brackets:
    unsigned idx = dev_tty.current_tty;
    menu[3*idx+1] = '[';
    menu[3*idx+3] = ']';
    // paint the menu:
    for (int i = 0; i < 80; ++i) {
      uint16_t d = color | menu[i];
      dev_tty.vga_shadow[i] = d;
      vga_buffer[i] = d;
    }
  }
}

void dev_tty_init() {
  memset(&dev_tty, 0, sizeof(dev_tty));
  for (int i = 0; i < N_TTYS; ++i) {
    dev_tty.ttys[i].id = i;
    //dev_tty.ttys[i].buffer = (uint16_t)malloc(VGA_WIDTH*TTY_HEIGHT);
    dev_tty.ttys[i].window_end = VGA_WIDTH*VGA_HEIGHT;
    dev_tty.ttys[i].screen_end = VGA_WIDTH*VGA_HEIGHT;
    dev_tty.ttys[i].color = make_vgaentry(0, make_color(COLOR_LIGHT_GREY,
                                                        COLOR_BLACK));
    uint16_t d = dev_tty.ttys[i].color | ' ';
    for (unsigned j = 0; j < VGA_WIDTH*VGA_HEIGHT; ++j) {
      dev_tty.ttys[i].buffer[j] = d;
    }
  }
  
  struct vfs_filesys filesys;
  memset(&filesys, 0, sizeof(filesys));
  filesys.open = MAKE_IDC_PTR(vfs_open, dev_tty_open);
  filesys.close = MAKE_IDC_PTR(vfs_close, dev_tty_close);
  filesys.read = MAKE_IDC_PTR(vfs_read, dev_tty_read);
  filesys.write = MAKE_IDC_PTR(vfs_write, dev_tty_write);
  filesys.lseek = MAKE_IDC_PTR(vfs_lseek, dev_tty_lseek);
  //filesys.fsync = MAKE_IDC_PTR(vfs_fsync, dev_tty_fsync);
  //filesys.ftruncate = MAKE_IDC_PTR(vfs_ftruncate, dev_tty_ftruncate);

  //pthread_cond_init(&dev_tty.cond, NULL);
  //pthread_mutex_init(&dev_tty.mutex, NULL);
  
  dev_register(NO_IDC, "tty0", &filesys); // current virtual console
  dev_register(NO_IDC, "tty1", &filesys); // 1st virtual console
  dev_register(NO_IDC, "tty2", &filesys); // 2nd virtual console
  dev_register(NO_IDC, "tty3", &filesys); // 3rd virtual console
};

void dev_tty_switch_to(unsigned tty_id) {
  // save old console.
  struct dev_tty_buffer* old_tty = &dev_tty.ttys[dev_tty.current_tty];
  unsigned index = old_tty->screen_end - WINDOW_SIZE;
  for (unsigned i = 0; i < WINDOW_SIZE; ++i, ++index) {
    uint16_t d = vga_buffer[i];
    if (dev_tty.menu_visible && i < 80) {
      d = dev_tty.menusave[i];
    }
    old_tty->buffer[index % MEMORY_SIZE] = d;
    dev_tty.vga_shadow[i] = d;
  }
  
  // save current cursor position.
  outb(0x3D4, 0x0F);
  unsigned position = inb(0x3D5);
  outb(0x3D4, 0x0E);
  position += inb(0x3D5) << 8;
  if (position != 0xffff) {
    old_tty->cursor_index = old_tty->screen_end - WINDOW_SIZE + position;
  }
  dev_tty.vga_copied = 1;

  // draw new console.
  if (tty_id != dev_tty.current_tty) {
    dev_tty.current_tty = tty_id;
    //pthread_cond_broadcast(&dev_tty.cond);
    update_vga_full(&dev_tty.ttys[tty_id]);
  }
}

void dev_tty_show_menu() {
  dev_tty.menu_visible = 1;
  for (int i = 0; i < 80; ++i) {
    dev_tty.menusave[i] = dev_tty.vga_shadow[i];
  }
  update_vga_full(&dev_tty.ttys[dev_tty.current_tty]);
}

void dev_tty_hide_menu() {
  dev_tty.menu_visible = 0;
  update_vga_full(&dev_tty.ttys[dev_tty.current_tty]);
}

__IDCIMPL__ int dev_tty_open(IDC_PTR, struct vfs_file* file, const char *name, int flags) {
  (void)(flags); //TODO: check flags.
  if (name[3] == '0') {
    file->internal = NULL;
  } else {
    unsigned idx = (unsigned)(name[3] - '1');
    file->internal = &dev_tty.ttys[idx];
  }
  if (!dev_tty.vga_copied) dev_tty_switch_to(0);
  return 0;
}

__IDCIMPL__ int dev_tty_close(IDC_PTR, struct vfs_file* file) {
  memset(&file->filesys, 0, sizeof(file->filesys));
  file->internal = NULL;
  return 0;
}

__IDCIMPL__ ssize_t dev_tty_read(IDC_PTR, struct vfs_file* file, void * buffer, size_t size) {
  struct dev_tty_buffer* tty = (struct dev_tty_buffer*)file->internal;
  if (tty == NULL) tty = &dev_tty.ttys[dev_tty.current_tty];
  uint8_t* cbuffer = (uint8_t*)buffer;

  // block until we are the active console
  //pthread_mutex_lock(&dev_tty.mutex);
  while (dev_tty.current_tty != tty->id) {
    //pthread_cond_wait(&dev_tty.cond, &dev_tty.mutex);
  }

  size_t bytes = 0;
  if (dev_tty.current_tty == tty->id) {
    while (bytes < size) {
      int kc = CORE_IDC(coresrv_kbd_getc);
      if (kc < 0) break;
      int c = keymap_code2char(keymap,
                               KBD_GETC_KEYCODE(kc), KBD_GETC_FLAGS(kc));
      if (c > 0) cbuffer[bytes++] = (uint8_t)c;
    }
  }
  //pthread_mutex_unlock(&dev_tty.mutex);
  return bytes;
}

__IDCIMPL__ ssize_t dev_tty_write(IDC_PTR, struct vfs_file* file, const void* data, size_t size) {
  struct dev_tty_buffer* tty = (struct dev_tty_buffer*)file->internal;
  if (tty == NULL) tty = &dev_tty.ttys[dev_tty.current_tty];
  uint16_t* buffer = tty->buffer;
  uint8_t* cdata = (uint8_t*)data;

  // Virtual buffer extends from 0 to infinity.
  // Real buffer extends from buffer_begin to buffer_end.
  // buffer_end = buffer_begin + VGA_WIDTH*TTY_HEIGHT.

  // Next character is written to cursor_index.
  // Active area extends from window_begin to window_end.
  // window_end = window_begin+VGA_WIDTH*VGA_HEIGHT.
  // cursor_index >= window_begin && cursor_index < window_end.
  // window_end = buffer_end.

  // If cursor goes past window_end, window_end += VGA_WIDTH.
  // The revealed line is cleared.
  
  // Index to pointer = buffer_base + index % MEMORY_SIZE.

  unsigned cursor_index = tty->cursor_index;
  unsigned window_begin = tty->window_end - WINDOW_SIZE;
  unsigned window_end = tty->window_end;
  unsigned min_cursor = cursor_index;
  unsigned max_cursor = cursor_index;
  uint16_t color = tty->color;
  unsigned eol;
  unsigned clear_line = 0;
  for (size_t i = 0; i < size; ++i) {
    uint8_t byte = cdata[i];
    switch (byte) {
    case '\b': // backspace.
      if (cursor_index > window_begin) {
        --cursor_index;
        buffer[cursor_index % MEMORY_SIZE] = color | ' ';
      }
      break;
    case '\n': // new line.
      eol = VGA_WIDTH - cursor_index % VGA_WIDTH;
      if (clear_line) {
        for (unsigned j = 0; j < eol; ++j) {
          buffer[(cursor_index + j) % MEMORY_SIZE] = color | ' ';
        }
      }
      cursor_index += eol;
      break;
    default: // printable chars.
      buffer[cursor_index % MEMORY_SIZE] = color | (uint16_t)byte;
      ++cursor_index;
      break;
    }

    if (cursor_index >= window_end) {
      window_end += VGA_WIDTH;
      window_begin = window_end - WINDOW_SIZE;
      clear_line = 1;
    }

    if (cursor_index < min_cursor) min_cursor = cursor_index;
    if (cursor_index > max_cursor) max_cursor = cursor_index;
  }

  if (clear_line) {
    eol = VGA_WIDTH - cursor_index % VGA_WIDTH;
    for (unsigned j = 0; j < eol; ++j) {
      buffer[(cursor_index + j) % MEMORY_SIZE] = color | ' ';
    }
  }

  tty->cursor_index = cursor_index;
  tty->screen_end = window_end;

  if (tty->window_end != window_end) {
    tty->window_end = window_end;
    update_vga_full(tty);
  } else {
    update_vga_part(tty, min_cursor, max_cursor - min_cursor);
  }

  return size;
}

__IDCIMPL__ off_t dev_tty_lseek(IDC_PTR, struct vfs_file* file, off_t offset, int where) {
  (void)(offset);//TODO implement
  struct dev_tty_buffer* tty = (struct dev_tty_buffer*)file->internal;
  if (tty == NULL) return (off_t)(-1);
  
  switch (where) {
  case SEEK_SET:
    break;
  case SEEK_CUR:
    break;
  case SEEK_END:
    break;
  }

  update_vga_cursor(tty);
  return (off_t)(-1);
}

