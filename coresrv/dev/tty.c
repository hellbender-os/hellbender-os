#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>
#include <errno.h>
#include <termios.h>

#include <kernel/kernel.h>
#include <kernel/io.h>
#include <kernel/vga.h>

#include <coresrv/kbd.h>
#include <coresrv/tty.h>
#include <coresrv/dev.h>
#include <coresrv/vfs.h>
#include <coresrv/rtc.h>

#define N_TTYS 3
#define TTY_HEIGHT 100

#define MEMORY_SIZE (VGA_WIDTH*TTY_HEIGHT)
#define WINDOW_SIZE (VGA_WIDTH*VGA_HEIGHT)
#define KBD_MAX_EVENTS 32
#define ESC_SEQ_MAX 15

struct dev_tty_kbd {
  kbd_event_t events[KBD_MAX_EVENTS];
  unsigned first_event;
  volatile unsigned last_event;
  sem_t event_sema; // posted for every event, waited for every read.
};

struct dev_tty_buffer {
  unsigned id;
  struct termios termios;
  struct dev_tty_kbd kbd;
  unsigned which_buffer; // 0 == main; 1 == alt.
  uint16_t main_buffer[VGA_WIDTH*VGA_HEIGHT];
  unsigned main_cursor;// offset of the cursor from top-left of the window.
  uint16_t alt_buffer[VGA_WIDTH*VGA_HEIGHT];
  unsigned alt_cursor;
  uint16_t color;
  char esc_sequence[ESC_SEQ_MAX+1];
  unsigned n_esc_bytes;
};

struct dev_tty {
  struct dev_tty_buffer ttys[N_TTYS];
  volatile unsigned current_tty;
  int magic_down;
  
  uint16_t vga_shadow[WINDOW_SIZE];
  unsigned vga_cursor_x;
  unsigned vga_cursor_y;
  unsigned cursor_visible;
  unsigned vga_copied;
  unsigned menu_visible;
  uint16_t menu[VGA_WIDTH];
} dev_tty;

extern uint16_t vga_buffer[];

static uint16_t* get_tty_row(struct dev_tty_buffer* tty, unsigned row) {
  switch (tty->which_buffer) {
  case 0:
    return tty->main_buffer + row * VGA_WIDTH;
  case 1:
    return tty->alt_buffer + row * VGA_WIDTH;
  default: return NULL;
  }
}

static unsigned get_tty_cursor(struct dev_tty_buffer* tty) {
  switch (tty->which_buffer) {
  case 0:
    return tty->main_cursor;
  case 1:
    return tty->alt_cursor;
  default: return 0xffff;
  }
}

static void set_tty_cursor(struct dev_tty_buffer* tty, unsigned cursor) {
  switch (tty->which_buffer) {
  case 0:
    tty->main_cursor = cursor;
    break;
  case 1:
    tty->alt_cursor = cursor;
    break;
  }
}

static void update_vga_cursor(struct dev_tty_buffer *tty) {
  if (tty->id != dev_tty.current_tty) return;

  unsigned position = get_tty_cursor(tty);
  if (position >= WINDOW_SIZE) {
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

  uint16_t* buffer = get_tty_row(tty, 0);
  for (; size; --size, ++index) {
    uint16_t d = buffer[index];
    if (dev_tty.menu_visible && index < 80) {
      d = dev_tty.menu[index];
    }
    if (d != dev_tty.vga_shadow[index]) {
      dev_tty.vga_shadow[index] = d;
      vga_buffer[index] = d;
    }
  }
  update_vga_cursor(tty);
}

static void update_vga_full(struct dev_tty_buffer *tty) {
  if (tty->id != dev_tty.current_tty) return;

  uint16_t* buffer = get_tty_row(tty, 0);
  for (unsigned index = 0; index < WINDOW_SIZE; ++index) {
    uint16_t d = buffer[index];
    if (dev_tty.menu_visible && index < 80) {
      d = dev_tty.menu[index];
    }
    if (d != dev_tty.vga_shadow[index]) {
      dev_tty.vga_shadow[index] = d;
      vga_buffer[index] = d;
    }
  }
  update_vga_cursor(tty);
}


static void switch_to_alt_buffer(struct dev_tty_buffer* tty) {
  tty->which_buffer = 1;
  uint16_t d = tty->color | ' ';
  for (unsigned j = 0; j < VGA_WIDTH*VGA_HEIGHT; ++j) {
    tty->alt_buffer[j] = d;
  }
  update_vga_full(tty);
}

static void switch_to_main_buffer(struct dev_tty_buffer* tty) {
  tty->which_buffer = 0;
  update_vga_full(tty);
}

static void erase_in_display(struct dev_tty_buffer* tty, unsigned cursor, int mode) {
  uint16_t* buffer = get_tty_row(tty, 0);
  uint16_t d = tty->color | ' ';
  switch (mode) {
  case 1:
    for (unsigned j = 0; j < cursor; ++j) {
      buffer[j] = d;
    }
    break;
  case 2:
    for (unsigned j = 0; j < WINDOW_SIZE; ++j) {
      buffer[j] = d;
    }
    break;
  default:
    for (unsigned j = cursor; j < WINDOW_SIZE; ++j) {
      buffer[j] = d;
    }
    break;
  }
  update_vga_full(tty);
}

static void erase_in_line(struct dev_tty_buffer* tty, unsigned cursor, int mode) {
  uint16_t* buffer = get_tty_row(tty, cursor / VGA_WIDTH);
  unsigned x = cursor % VGA_WIDTH;
  uint16_t d = tty->color | ' ';
  switch (mode) {
  case 1:
    for (unsigned j = 0; j < x; ++j) {
      buffer[j] = d;
    }
    break;
  case 2:
    for (unsigned j = 0; j < VGA_WIDTH; ++j) {
      buffer[j] = d;
    }
    break;
  default:
    for (unsigned j = x; j < VGA_WIDTH; ++j) {
      buffer[j] = d;
    }
    break;
  }
  update_vga_full(tty);
}

static int decode_control_seq(struct dev_tty_buffer* tty, char byte, unsigned *cursor) {
  if (tty->n_esc_bytes == 0) {
    switch (byte) {
    case 7:
      rtc_beep(NO_IDC, 600, 10);
      return 1;
    case 27:
      tty->esc_sequence[tty->n_esc_bytes++] = byte;
      return 1;
    default:
      return 0;
    }
  } else {
    if (tty->n_esc_bytes < ESC_SEQ_MAX) {
      tty->esc_sequence[tty->n_esc_bytes++] = byte;
    }
    if (byte >= 64 && byte != '[') {
      tty->esc_sequence[tty->n_esc_bytes] = 0;
      // detect code:
      if (false) { // just so that every case below are the same.
      } else if (strcmp(tty->esc_sequence, "\033[?1049h") == 0) {
        switch_to_alt_buffer(tty);
        *cursor = get_tty_cursor(tty);

      } else if (strcmp(tty->esc_sequence, "\033[?1049l") == 0) {
        switch_to_main_buffer(tty);
        *cursor = get_tty_cursor(tty);

      } else if (byte == 'H') {
        int y = atoi(tty->esc_sequence+2);
        int x = atoi(strchr(tty->esc_sequence, ';') + 1);
        *cursor = (y - 1) * VGA_WIDTH + x - 1;
      } else if (byte == 'J') {
        int mode = atoi(tty->esc_sequence+2);
        erase_in_display(tty, *cursor, mode);
      } else if (byte == 'K') {
        int mode = atoi(tty->esc_sequence+2);
        erase_in_line(tty, *cursor, mode);
      } else {
        // [7m => inversible (colors)
        // [0m => default (colors)
        BREAK;
      }
      
      tty->n_esc_bytes = 0;
    }
    return 1;
  }
}


void dev_tty_init() {
  memset(&dev_tty, 0, sizeof(dev_tty));
  for (int i = 0; i < N_TTYS; ++i) {
    dev_tty.ttys[i].id = i;
    sem_init(&dev_tty.ttys[i].kbd.event_sema, 1, 0);
    dev_tty.ttys[i].color = make_vgaentry(0, make_color(COLOR_LIGHT_GREY,
                                                        COLOR_BLACK));
    uint16_t d = dev_tty.ttys[i].color | ' ';
    for (unsigned j = 0; j < VGA_WIDTH*VGA_HEIGHT; ++j) {
      dev_tty.ttys[i].main_buffer[j] = d;
      dev_tty.ttys[i].alt_buffer[j] = d;
    }
  }
  
  struct vfs_filesys filesys = (struct vfs_filesys) {
    .create    = NULL,
    .open      = MAKE_IDC_PTR(vfs_open, dev_tty_open),
    .close     = MAKE_IDC_PTR(vfs_close, dev_tty_close),
    .read      = MAKE_IDC_PTR(vfs_read, dev_tty_read),
    .write     = MAKE_IDC_PTR(vfs_write, dev_tty_write),
    .lseek     = MAKE_IDC_PTR(vfs_lseek, dev_tty_lseek),
    .fsync     = NULL, // MAKE_IDC_PTR(vfs_fsync, dev_tty_fsync),
    .ftruncate = NULL,
    .fstat     = NULL, // MAKE_IDC_PTR(vfs_fstat, dev_tty_fstat),
    .link      = NULL,
    .unlink    = NULL,
    .termios   = MAKE_IDC_PTR(vfs_termios, dev_tty_termios),
    .internal  = NULL,
  };
  
  dev_register(NO_IDC, "tty0", &filesys); // current virtual console
  dev_register(NO_IDC, "tty1", &filesys); // 1st virtual console
  dev_register(NO_IDC, "tty2", &filesys); // 2nd virtual console
  dev_register(NO_IDC, "tty3", &filesys); // 3rd virtual console
};

static void dev_tty_save() {
  // save old console.
  struct dev_tty_buffer* tty = &dev_tty.ttys[dev_tty.current_tty];
  uint16_t* buffer = get_tty_row(tty, 0);
  for (unsigned index = 0; index < WINDOW_SIZE; ++index) {
    uint16_t d = vga_buffer[index];
    buffer[index] = d;
    dev_tty.vga_shadow[index] = d;
  }
  
  // save current cursor position.
  outb(0x3D4, 0x0F);
  unsigned position = inb(0x3D5);
  outb(0x3D4, 0x0E);
  position += inb(0x3D5) << 8;
  tty->main_cursor = position;

  dev_tty.vga_copied = 1;
}

static void dev_tty_show_menu() {
  // draw menu string:
  uint16_t color = make_vgaentry(0, make_color(COLOR_WHITE,
                                               COLOR_BLUE));
  char menu[81] = ("  1  2  3                               "
                   "         16M/133M  L:1.3/0.7/0.5  xx:xx ");
  // draw current time:
  time_t rawtime = time(NULL);
  struct tm* ltime = localtime(&rawtime);
  sprintf(menu+74, "%02i:%02i", ltime->tm_hour, ltime->tm_min);
  // draw brackets:
  unsigned idx = dev_tty.current_tty;
  menu[3*idx+1] = '[';
  menu[3*idx+3] = ']';
  // setup the menu buffer:
  for (int i = 0; i < 80; ++i) {
    uint16_t d = color | menu[i];
    dev_tty.menu[i] = d;
  }

  // paint the menu:
  dev_tty.menu_visible = 1;
  update_vga_full(&dev_tty.ttys[dev_tty.current_tty]);
}

static void dev_tty_hide_menu() {
  dev_tty.menu_visible = 0;
  update_vga_full(&dev_tty.ttys[dev_tty.current_tty]);
}

static void dev_tty_switch_to(unsigned tty_id) {
  // draw new console.
  if (tty_id != dev_tty.current_tty) {
    dev_tty.current_tty = tty_id;
    // make sure menu is updated:
    if (dev_tty.menu_visible) dev_tty_show_menu();
    else update_vga_full(&dev_tty.ttys[tty_id]);
  }
}

static int magic_key(struct kbd_event *event) {
  if (event->flags == (KBD_FLAG_LSHIFT+KBD_FLAG_LCTRL+KBD_FLAG_LALT)) {
    if (!dev_tty.magic_down) {
      dev_tty_show_menu();
      dev_tty.magic_down = 1;
    }
    
    if (event->event_type == KBD_EVENT_KEYDOWN) {
      int c = event->plain_c;
      switch (c) {
      case '1': // switch between virtual terminals.
      case '2':
      case '3':
        {
          unsigned tty_id = c - '1';
          dev_tty_switch_to(tty_id);
        }
        break;
      }
    }
    return 1;

  } else {
    if (dev_tty.magic_down) {
      dev_tty_hide_menu();
      dev_tty.magic_down = 0;
    }
    return 0;
  }
}

int dev_tty_post(kbd_event_t *event) {
  if (magic_key(event)) return 0;

  struct dev_tty_buffer* tty = &dev_tty.ttys[dev_tty.current_tty];
  struct dev_tty_kbd* kbd = &tty->kbd;
  
  unsigned event_idx = (kbd->last_event + 1) % KBD_MAX_EVENTS;
  if (event_idx != kbd->first_event) {
    kbd->events[kbd->last_event] = *event;
    BARRIER;
    kbd->last_event = event_idx;
    sem_post(&kbd->event_sema);
    return 0;
  } else {
    return -1;
  }
}

__IDCIMPL__ int dev_tty_open(IDC_PTR, struct vfs_file* file, const char *name, int flags) {
  (void)(flags); //TODO: check flags.
  if (name[3] == '0') {
    file->internal = NULL;
  } else {
    unsigned idx = (unsigned)(name[3] - '1');
    file->internal = &dev_tty.ttys[idx];
  }
  if (!dev_tty.vga_copied) dev_tty_save();
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
  struct dev_tty_kbd* kbd = &tty->kbd;
  uint8_t* cbuffer = (uint8_t*)buffer;

  // block until we have events:
  size_t bytes = 0;
  while (bytes < size) {
    sem_wait(&kbd->event_sema);
    unsigned event_idx = kbd->first_event;
    kbd_event_t event = kbd->events[event_idx];
    BARRIER;
    kbd->first_event = (event_idx + 1) % KBD_MAX_EVENTS;
    if (event.event_type & KBD_EVENT_KEYDOWN) {
      int c = event.real_c;
      if (c > 0) cbuffer[bytes++] = (uint8_t)c;
    }
  }
  return bytes;
}

__IDCIMPL__ ssize_t dev_tty_write(IDC_PTR, struct vfs_file* file, const void* data, size_t size) {
  struct dev_tty_buffer* tty = (struct dev_tty_buffer*)file->internal;
  if (tty == NULL) tty = &dev_tty.ttys[dev_tty.current_tty];
  uint8_t* cdata = (uint8_t*)data;

  unsigned cursor_index = get_tty_cursor(tty);
  unsigned min_cursor = cursor_index;
  unsigned max_cursor = cursor_index;
  uint16_t color = tty->color;
  unsigned eol;
  unsigned full = 0;
  for (size_t i = 0; i < size; ++i) {
    uint8_t byte = cdata[i];
    if (decode_control_seq(tty, (char)byte, &cursor_index)) continue;
    uint16_t* buffer = get_tty_row(tty, 0);
       
    switch (byte) {
    case '\b': // backspace.
      if (cursor_index > 0) {
        buffer[--cursor_index] = color | ' ';
      }
      break;
    case '\n': // new line.
      eol = VGA_WIDTH - cursor_index % VGA_WIDTH;
      cursor_index += eol;
      break;
    default: // printable chars.
      buffer[cursor_index++] = color | (uint16_t)byte;
      break;
    }

    if (cursor_index < min_cursor) min_cursor = cursor_index;
    if (cursor_index > max_cursor) max_cursor = cursor_index;
    if (cursor_index == WINDOW_SIZE) {
      cursor_index -= VGA_WIDTH;
      full = 1;
      memmove(buffer, buffer + VGA_WIDTH, (WINDOW_SIZE - VGA_WIDTH) * 2);
      uint16_t *ptr = get_tty_row(tty, VGA_HEIGHT-1);
      for (unsigned j = 0; j < VGA_WIDTH; ++j) {
        ptr[j] = color | ' ';
      }
    }
  }

  set_tty_cursor(tty, cursor_index);
  if (full) {
    update_vga_full(tty);
  } else {
    update_vga_part(tty, min_cursor, max_cursor - min_cursor);
  }
  return size;
}

__IDCIMPL__ off_t dev_tty_lseek(IDC_PTR, struct vfs_file* file, off_t offset, int where) {
  (void)(offset);//TODO implement
   struct dev_tty_buffer* tty = (struct dev_tty_buffer*)file->internal;
  if (tty == NULL) tty = &dev_tty.ttys[dev_tty.current_tty];
  
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

__IDCIMPL__ int dev_tty_termios(IDC_PTR, struct vfs_file* file, struct termios* termios, int cmd) {
  struct dev_tty_buffer* tty = (struct dev_tty_buffer*)file->internal;
  if (tty == NULL) tty = &dev_tty.ttys[dev_tty.current_tty];

  switch (cmd) {
  case VFS_TERMIOS_GET:
    memcpy(termios, &tty->termios, sizeof(struct termios));
    return 0;

  case VFS_TERMIOS_SET:
    memcpy(&tty->termios, termios, sizeof(struct termios));
    return 0;

  default:
    errno = EINVAL;
    return -1;
  }
}
