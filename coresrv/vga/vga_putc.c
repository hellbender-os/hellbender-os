#include <stdint.h>
#include <kernel/kernel.h>
#include <kernel/io.h>
#include <coresrv/vga.h>

extern uint16_t vga_buffer[];

static void update_cursor(int row, int col)
 {
   unsigned short position=(row*VGA_WIDTH) + col;
   // cursor LOW port to vga INDEX register
   outb(0x3D4, 0x0F);
   outb(0x3D5, (unsigned char)(position&0xFF));
   // cursor HIGH port to vga INDEX register
   outb(0x3D4, 0x0E);
   outb(0x3D5, (unsigned char )((position>>8)&0xFF));
 }

__IDCIMPL__ void coresrv_vga_putc(IDC_PTR, unsigned x, unsigned y, unsigned cursorx, unsigned c, unsigned color) {
  if (x < VGA_WIDTH && y < VGA_HEIGHT) {
    vga_buffer[y * VGA_WIDTH + x] = make_vgaentry(c, color);
    update_cursor(y, cursorx);
  }
}
