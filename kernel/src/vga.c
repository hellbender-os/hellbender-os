#include "vga.h"
#include "multiboot.h"
#include "kernel.h"
#include "page.h"

#include <hellbender/inline.h>

#include "vga_font.h"
#include "vga_bits.h"

INLINE void vga_inc() {
  if (++vga.txt_col == vga.txt_cols) {
    vga.txt_col = 0;
    if (++vga.txt_row == vga.txt_rows) {
      vga.txt_row = 0; // TODO: scroll instead of wrapping
    }
    // TODO: clear the line
    vga_set_cursor(vga.txt_row, vga.txt_col);
  } else {
    vga.txt_cursor++;
    vga.fb_cursor += vga.col_step;
  }
}

void vga_set_cursor(unsigned row, unsigned col) {
  if (row >= vga.txt_rows) row = vga.txt_rows - 1;
  if (col >= vga.txt_cols) col = vga.txt_cols - 1;
  vga.txt_cursor = vga.txt_shadow + row * vga.txt_cols + col;
  vga.fb_cursor = vga.fb + row * vga.row_step + col * vga.col_step;
  vga.txt_row = row;
  vga.txt_col = col;
}

void vga_get_cursor(unsigned *row, unsigned *col) {
  *row = vga.txt_row;
  *col = vga.txt_col;
}

void vga_newl() {
  vga.txt_col = vga.txt_cols - 1;
  vga_inc();
}

void vga_set_fg_rgb24(uint32_t rgb) {
  vga.fg = rgb;
  uint32_t r = rgb & 0xff;
  uint32_t g = (rgb & 0xff00)>>8;
  uint32_t b = (rgb & 0xff0000)>>16;
  vga.fg24[0] = r | (g<<8) | (b<<16) | (r<<24);
  vga.fg24[1] = g | (b<<8) | (r<<16) | (g<<24);
  vga.fg24[2] = b | (r<<8) | (g<<16) | (b<<24);
}

void vga_set_bg_rgb24(uint32_t rgb) {
  vga.bg = rgb;
  uint32_t r = rgb & 0xff;
  uint32_t g = (rgb & 0xff00)>>8;
  uint32_t b = (rgb & 0xff0000)>>16;
  vga.bg24[0] = r | (g<<8) | (b<<16) | (r<<24);
  vga.bg24[1] = g | (b<<8) | (r<<16) | (g<<24);
  vga.bg24[2] = b | (r<<8) | (g<<16) | (b<<24);
}

void vga_set_fg_bgr24(uint32_t rgb) {
  vga.fg = rgb;
  uint32_t r = rgb & 0xff;
  uint32_t g = (rgb & 0xff00)>>8;
  uint32_t b = (rgb & 0xff0000)>>16;
  vga.fg24[0] = b | (g<<8) | (r<<16) | (b<<24);
  vga.fg24[1] = g | (r<<8) | (b<<16) | (g<<24);
  vga.fg24[2] = r | (b<<8) | (g<<16) | (r<<24);
}

void vga_set_bg_bgr24(uint32_t rgb) {
  vga.bg = rgb;
  uint32_t r = rgb & 0xff;
  uint32_t g = (rgb & 0xff00)>>8;
  uint32_t b = (rgb & 0xff0000)>>16;
  vga.bg24[0] = b | (g<<8) | (r<<16) | (b<<24);
  vga.bg24[1] = g | (r<<8) | (b<<16) | (g<<24);
  vga.bg24[2] = r | (b<<8) | (g<<16) | (r<<24);
}

void vga_putc_24(char c) {
  if ((uint32_t)c != *vga.txt_shadow >> 24) {
    uint8_t *glyph = vga_font[((unsigned)c & 127) - 32];
    uint32_t *dst = (uint32_t*)vga.fb_cursor;
    for (int y = 0; y < 16; ++y) {
      uint8_t bits = glyph[y];
      uint32_t *hi = vga_bits24[bits>>4];
      dst[0] = (hi[0] & vga.fg24[0]) | ((~hi[0]) & vga.bg24[0]);
      dst[1] = (hi[1] & vga.fg24[1]) | ((~hi[1]) & vga.bg24[1]);
      dst[2] = (hi[2] & vga.fg24[2]) | ((~hi[2]) & vga.bg24[2]);
      uint32_t *lo = vga_bits24[bits&0xf];
      dst[3] = (lo[0] & vga.fg24[0]) | ((~lo[0]) & vga.bg24[0]);
      dst[4] = (lo[1] & vga.fg24[1]) | ((~lo[1]) & vga.bg24[1]);
      dst[5] = (lo[2] & vga.fg24[2]) | ((~lo[2]) & vga.bg24[2]);
      dst += vga.stride32;
    }
  }
  vga_inc();
}

void vga_puts_24(const char* str) {
  for (char c = *str; c; c = *++str) {
    vga_putc_24(c);
  }
}

void vga_set_fg_rgb32(uint32_t rgb) {
  vga.fg = rgb;
  vga.fg32 = ((uint64_t)rgb) | (((uint64_t)rgb)<<32);
}

void vga_set_bg_rgb32(uint32_t rgb) {
  vga.bg = rgb;
  vga.bg32 = ((uint64_t)rgb) | (((uint64_t)rgb)<<32);
}

void vga_set_fg_bgr32(uint32_t rgb) {
  vga.fg = rgb;
  uint64_t r = (uint64_t)(rgb & 0xff);
  uint64_t g = (uint64_t)((rgb & 0xff00)>>8);
  uint64_t b = (uint64_t)((rgb & 0xff0000)>>16);
  vga.fg32 = b | (g<<8) | (r<<16) | (b<<32) | (g<<40) | (r<<48);
}

void vga_set_bg_bgr32(uint32_t rgb) {
  vga.bg = rgb;
  uint64_t r = (uint64_t)(rgb & 0xff);
  uint64_t g = (uint64_t)((rgb & 0xff00)>>8);
  uint64_t b = (uint64_t)((rgb & 0xff0000)>>16);
  vga.bg32 = b | (g<<8) | (r<<16) | (b<<32) | (g<<40) | (r<<48);
}

void vga_putc_32(char c) {
  if ((uint32_t)c != *vga.txt_shadow >> 24) {
    uint8_t *glyph = vga_font[((unsigned)c & 127) - 32];
    uint64_t *dst = (uint64_t*)vga.fb_cursor;
    for (int y = 0; y < 16; ++y) {
      uint8_t bits = glyph[y];
      uint64_t *hi = vga_bits32[bits>>4];
      dst[0] = (hi[0] & vga.fg32) | ((~hi[0]) & vga.bg32);
      dst[1] = (hi[1] & vga.fg32) | ((~hi[1]) & vga.bg32);
      uint64_t *lo = vga_bits32[bits&0xf];
      dst[2] = (lo[0] & vga.fg32) | ((~lo[0]) & vga.bg32);
      dst[3] = (lo[1] & vga.fg32) | ((~lo[1]) & vga.bg32);
      dst += vga.stride64;
    }
  }
  vga_inc();
}

void vga_puts_32(const char* str) {
  for (char c = *str; c; c = *++str) {
    vga_putc_32(c);
  }
}

void vga_init() {
  vbe_mode_info_t *mode = &multiboot_data.mode_info;
  // map linear frame buffer to kernel.
  {
    uint64_t ptr = (uint64_t)mode->phys_base_ptr;
    for (int64_t bytes = mode->y_resolution * mode->bytes_per_scanline;
         bytes > 0; ptr += TABLE_SIZE, bytes -= TABLE_SIZE) {
      page_map_2M(kernel_p2v(ptr), ptr, PAGE_KERNEL_RW);
    }
  }

  // glyphs are 8 pixels wide and 16 pixels tall.
  vga.txt_cols = mode->x_resolution / 8;
  vga.txt_rows = mode->y_resolution / 16;
  vga.txt_shadow = heap_alloc_clear(vga.txt_rows * vga.txt_cols * 4);
  vga.fb = kernel_p2v(mode->phys_base_ptr);
  vga.fb_size = (uint64_t)mode->y_resolution * (uint64_t)mode->bytes_per_scanline;
  vga.row_step = 16 * (unsigned)mode->bytes_per_scanline;
  vga.col_step = (unsigned)mode->bits_per_pixel; // == bpp/8*glyph_width
  vga.stride32 = (unsigned)mode->bytes_per_scanline / 4;
  vga.stride64 = (unsigned)mode->bytes_per_scanline / 8;

  // check that multiboot_data has 24 or 32 bit linear frame buffer
  // and that RGB masks are good.
  if (mode->red_mask_size != 8
      || mode->green_mask_size != 8
      || mode->blue_mask_size != 8) kernel_panic();
  int order = 0; // +1 == rgb, -1 == bgr
  if (mode->red_field_position == 0 && 
      mode->green_field_position == 8 &&
      mode->blue_field_position == 16) order = 1;
  else if (mode->red_field_position == 16 && 
           mode->green_field_position == 8 &&
           mode->blue_field_position == 0) order = -1;
  else kernel_panic();
  if (mode->bits_per_pixel == 24) {
    vga.op.putc = vga_putc_24;
    vga.op.puts = vga_puts_24;
    vga.op.set_fg = order > 0 ? vga_set_fg_rgb24 : vga_set_fg_bgr24;
    vga.op.set_bg = order > 0 ? vga_set_bg_rgb24 : vga_set_bg_bgr24;
  } else if (mode->bits_per_pixel == 32) {
    vga.op.putc = vga_putc_32;
    vga.op.puts = vga_puts_32;
    vga.op.set_fg = order > 0 ? vga_set_fg_rgb32 : vga_set_fg_bgr32;
    vga.op.set_bg = order > 0 ? vga_set_bg_rgb32 : vga_set_bg_bgr32;
  } else {
    kernel_panic();
  }

  vga_set_fg(VGA_RGB(200,200,200));
  vga_set_bg(VGA_RGB(  0,  0,  0));
  vga_set_cursor(0, 0);
}
