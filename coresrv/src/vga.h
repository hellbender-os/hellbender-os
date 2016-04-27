#ifndef __HELLBENDER_CORESRV_VGA_H__
#define __HELLBENDER_CORESRV_VGA_H__

#include <stdint.h>

enum vga_color
{
	COLOR_BLACK = 0,
	COLOR_BLUE = 1,
	COLOR_GREEN = 2,
	COLOR_CYAN = 3,
	COLOR_RED = 4,
	COLOR_MAGENTA = 5,
	COLOR_BROWN = 6,
	COLOR_LIGHT_GREY = 7,
	COLOR_DARK_GREY = 8,
	COLOR_LIGHT_BLUE = 9,
	COLOR_LIGHT_GREEN = 10,
	COLOR_LIGHT_CYAN = 11,
	COLOR_LIGHT_RED = 12,
	COLOR_LIGHT_MAGENTA = 13,
	COLOR_LIGHT_BROWN = 14,
	COLOR_WHITE = 15,
};

#define VGA_COLOR(fg, bg) (((uint8_t)fg) | (((uint8_t)bg) << 4))
#define VGA_ENTRY(ch, co) (((uint16_t)ch) | (((uint16_t)co) << 8))
#define WHITE_ON_BLACK (VGA_COLOR(COLOR_WHITE, COLOR_BLACK))

#define VGA_WIDTH 80
#define VGA_HEIGHT 25

//static uint16_t* const VGA_MEMORY = (uint16_t*)(_vga_base);
__attribute__((always_inline)) inline
uint16_t* VGA_MEMORY() {
  register uint16_t *ptr;
  asm ("lea _vga_base(%%rip), %0"
       : "=r"(ptr));
  return ptr;
}
#define VGA_MEMORY_SIZE 0x20000

#define VGA_AT(row, col) (VGA_MEMORY()[row*(VGA_WIDTH)+col])

#endif
