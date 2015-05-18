#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/keymap.h>

#include <kernel/kernel.h>
#include <kernel/io.h>
#include <kernel/vga.h>

#include <coresrv/kbd.h>
#include <coresrv/tty.h>
#include <coresrv/dev.h>

#define N_TTYS 3
#define TTY_HEIGHT 100

struct dev_tty_buffer {
  unsigned id;
  uint16_t buffer[VGA_WIDTH*TTY_HEIGHT];
  unsigned top_index; // beginning of the topmost line in the buffer.
  unsigned bottom_index; // end of the bottomost line.
  unsigned cursor_index; // the line the cursor is at.
  unsigned cursor_offset; // the cursor position on the line.
  unsigned window_index; // the first visible line.
  uint16_t color;
};

struct dev_tty {
  struct dev_tty_buffer ttys[N_TTYS];
  unsigned current_tty;
  uint16_t vga_shadow[VGA_WIDTH*VGA_HEIGHT];
  unsigned vga_cursor_x;
  unsigned vga_cursor_y;
} dev_tty;

extern uint16_t vga_buffer[];

static void update_vga_cursor(struct dev_tty_buffer *tty) {
  if (tty->id != dev_tty.current_tty) return;

  unsigned position = tty->cursor_index + tty->cursor_offset;
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
  
  for (; size; --size) {
    if (index > tty->window_index) {
      unsigned vga_index = index - tty->window_index;
      uint16_t d = tty->buffer[index];
      if (d != dev_tty.vga_shadow[vga_index]) {
        dev_tty.vga_shadow[vga_index] = d;
        vga_buffer[vga_index] = d;
      }
    }
    if (++index == VGA_WIDTH*TTY_HEIGHT) index = 0;
  }
  update_vga_cursor(tty);
}

static void update_vga_full(struct dev_tty_buffer *tty) {
  if (tty->id != dev_tty.current_tty) return;

  unsigned index = tty->window_index;
  unsigned vga_index = 0;
  for (unsigned y = 0; y < VGA_HEIGHT; ++y) {
    for (unsigned x = 0; x < VGA_WIDTH; ++x, ++index, ++vga_index) {
      uint16_t d = tty->buffer[index];
      if (d != dev_tty.vga_shadow[vga_index]) {
        dev_tty.vga_shadow[vga_index] = d;
        vga_buffer[vga_index] = d;
      }
    }
    if (index == VGA_WIDTH*TTY_HEIGHT) index = 0;
  }
  update_vga_cursor(tty);
}

void dev_tty_init() {
  memset(&dev_tty, 0, sizeof(dev_tty));
  for (int i = 0; i < N_TTYS; ++i) {
    dev_tty.ttys[i].id = i;
    //dev_tty.ttys[i].buffer = (uint16_t)malloc(VGA_WIDTH*TTY_HEIGHT);
    dev_tty.ttys[i].bottom_index = VGA_WIDTH*TTY_HEIGHT;
    dev_tty.ttys[i].color = make_vgaentry(0, make_color(COLOR_WHITE,
                                                        COLOR_BLACK));
    uint16_t d = dev_tty.ttys[i].color | ' ';
    for (unsigned j = 0; j < VGA_WIDTH*VGA_HEIGHT; ++j) {
      dev_tty.ttys[i].buffer[j] = d;
    }
  }
  dev_tty_switch_to(0); // makes a copy of the VGA buffer.
  
  struct vfs_filesys filesys;
  memset(&filesys, 0, sizeof(filesys));
  filesys.open = MAKE_IDC_PTR(vfs_open, dev_tty_open);
  filesys.close = MAKE_IDC_PTR(vfs_close, dev_tty_close);
  filesys.read = MAKE_IDC_PTR(vfs_read, dev_tty_read);
  filesys.write = MAKE_IDC_PTR(vfs_write, dev_tty_write);
  filesys.lseek = MAKE_IDC_PTR(vfs_lseek, dev_tty_lseek);
  //filesys.fsync = MAKE_IDC_PTR(vfs_fsync, dev_tty_fsync);
  //filesys.ftruncate = MAKE_IDC_PTR(vfs_ftruncate, dev_tty_ftruncate);

  dev_register(NO_IDC, "tty0", &filesys); // current virtual console
  dev_register(NO_IDC, "tty1", &filesys); // 1st virtual console
  dev_register(NO_IDC, "tty2", &filesys); // 2nd virtual console
  dev_register(NO_IDC, "tty3", &filesys); // 3rd virtual console
};

void dev_tty_switch_to(unsigned tty_id) {
  // save old console.
  struct dev_tty_buffer* old_tty = &dev_tty.ttys[dev_tty.current_tty];
  for (unsigned j = 0; j < VGA_WIDTH*VGA_HEIGHT; ++j) {
    uint16_t d = vga_buffer[j];
    old_tty->buffer[j] = d;
    dev_tty.vga_shadow[j] = d;
  }
  // save current cursor position.
  outb(0x3D4, 0x0F);
  unsigned position = inb(0x3D5);
  outb(0x3D4, 0x0E);
  position += inb(0x3D5) << 8;
  old_tty->cursor_offset = position % VGA_WIDTH;
  old_tty->cursor_index = position - old_tty->cursor_offset;

  // draw new console.
  if (tty_id != dev_tty.current_tty) {
    dev_tty.current_tty = tty_id;
    update_vga_full(&dev_tty.ttys[tty_id]);
  }
}

__IDCIMPL__ int dev_tty_open(IDC_PTR, struct vfs_file* file, const char *name, int flags) {
  (void)(flags); //TODO: check flags.
  unsigned idx;
  if (name[3] == '0') {
    idx = dev_tty.current_tty;
  } else {
    idx = (unsigned)(name[3] - '1');
  }
  file->internal = &dev_tty.ttys[idx];
  return 0;
}

__IDCIMPL__ int dev_tty_close(IDC_PTR, struct vfs_file* file) {
  memset(&file->filesys, 0, sizeof(file->filesys));
  file->internal = NULL;
  return 0;
}

__IDCIMPL__ ssize_t dev_tty_read(IDC_PTR, struct vfs_file* file, void * buffer, size_t size) {
  struct dev_tty_buffer* tty = (struct dev_tty_buffer*)file->internal;
  uint8_t* cbuffer = (uint8_t*)buffer;
  if (dev_tty.current_tty == tty->id) {
    size_t bytes = 0;
    while (bytes < size) {
      int kc = CORE_IDC(coresrv_kbd_getc);
      if (kc < 0) break;
      int c = keymap_code2char(keymap,
                               KBD_GETC_KEYCODE(kc), KBD_GETC_FLAGS(kc));
      if (c > 0) cbuffer[bytes++] = (uint8_t)c;
    }
    return bytes;
    
  } else {
    // block until console change?
    return 0;
  }
}

__IDCIMPL__ ssize_t dev_tty_write(IDC_PTR, struct vfs_file* file, const void* data, size_t size) {
  struct dev_tty_buffer* tty = (struct dev_tty_buffer*)file->internal;
  uint16_t* buffer = tty->buffer;
  unsigned start_index = tty->cursor_index + tty->cursor_offset;
  uint8_t* cdata = (uint8_t*)data;

  // not enough to reach end of line 
  //  => copy data, update vga from that part.
  unsigned till_eol = VGA_WIDTH - tty->cursor_offset;
  if (size < till_eol) {
    uint16_t* start_ptr = buffer + start_index;
    for (size_t i = 0; i < size; ++i) {
      start_ptr[i] = tty->color | (uint16_t)(cdata[i]);
    }
    tty->cursor_offset += size;
    update_vga_part(tty, start_index, size);
    
  } else {
    // enough to fill the line: fill the first line.
    unsigned current_index = start_index;
    size_t remaining = size;
    int set_bottom = 0;
    {
      uint16_t* start_ptr = buffer + current_index;
      for (size_t i = 0; i < till_eol; ++i) {
        start_ptr[i] = tty->color | (uint16_t)(cdata[i]);
      }
      remaining -= till_eol;
      data += till_eol;
      current_index += till_eol;
      if (current_index == VGA_WIDTH*TTY_HEIGHT) current_index = 0;
      if (current_index == tty->bottom_index) set_bottom = 1;
    }
    // then fill line by line.
    while (remaining >= VGA_WIDTH) {
      uint16_t* start_ptr = buffer + current_index;
      for (size_t i = 0; i < VGA_WIDTH; ++i) {
        start_ptr[i] = tty->color | (uint16_t)(cdata[i]);
      }
      remaining -= VGA_WIDTH;
      data += VGA_WIDTH;
      current_index += VGA_WIDTH;
      if (current_index == VGA_WIDTH*TTY_HEIGHT) current_index = 0;
      if (current_index == tty->bottom_index) set_bottom = 1;
    }
    // then copy the last line; and clear till the end of line.
    {
      uint16_t* start_ptr = buffer + current_index;
      for (size_t i = 0; i < remaining; ++i) {
        start_ptr[i] = tty->color | (uint16_t)(cdata[i]);
      }
      for (size_t i = remaining; i < VGA_WIDTH; ++i) {
        start_ptr[i] = tty->color | (uint16_t)(' ');
      }
    }
    // update bottom, cursor, and window.
    tty->cursor_index = current_index;
    tty->cursor_offset = remaining;
    if (set_bottom) {
      tty->bottom_index = current_index + VGA_WIDTH;
      if (tty->bottom_index == VGA_WIDTH*TTY_HEIGHT) tty->bottom_index = 0;
      tty->window_index = (VGA_WIDTH*TTY_HEIGHT
                           + tty->bottom_index
                           - VGA_WIDTH*VGA_HEIGHT) % (VGA_WIDTH*TTY_HEIGHT);
      update_vga_full(tty);
    } else {
      update_vga_part(tty, start_index, size);
    }
  }

  return size;
}

__IDCIMPL__ off_t dev_tty_lseek(IDC_PTR, struct vfs_file* file, off_t offset, int where) {
  struct dev_tty_buffer* tty = (struct dev_tty_buffer*)file->internal;

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

