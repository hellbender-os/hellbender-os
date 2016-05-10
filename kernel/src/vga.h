#ifndef __HELLBENDER_KERNEL_VGA_H__
#define __HELLBENDER_KERNEL_VGA_H__

#include "config.h"
#include <hellbender/inline.h>

#include <stdint.h>

#define VGA_RGB(r,g,b) (((r)&0xff)|(((g)&0xff)<<8)|(((b)&0xff)<<16))

struct vga {
  uint32_t *txt_shadow; // text buffer of rows*cols; red,green<<8,blue<<16,char<<24
  uint8_t *fb;          // rgb24 or rgb32 of rows*row_step
  uint64_t fb_size;
  unsigned txt_rows, txt_cols;
  unsigned txt_row, txt_col;
  uint32_t *txt_cursor;
  unsigned row_step, col_step;
  unsigned stride32, stride64; // how many dwords/qwords per scanline
  uint8_t *fb_cursor;
  uint32_t fg, bg;
  uint32_t fg24[3], bg24[3];
  uint64_t fg32, bg32;

  struct vga_op {
    void (*set_fg)(uint32_t rgb);
    void (*set_bg)(uint32_t rgb);
    void (*putc)(char c);
    void (*puts)(const char* str);
  } op;
} vga;

void vga_init();

void vga_set_cursor(unsigned row, unsigned col);

void vga_get_cursor(unsigned *row, unsigned *col);

void vga_newl();

INLINE void vga_set_fg(uint32_t rgb) {
  vga.op.set_fg(rgb);
}

INLINE void vga_set_bg(uint32_t rgb) {
  vga.op.set_bg(rgb);
}

INLINE void vga_putc(char c) {
  vga.op.putc(c);
}

INLINE void vga_puts(const char* str) {
  vga.op.puts(str);
}

#endif
