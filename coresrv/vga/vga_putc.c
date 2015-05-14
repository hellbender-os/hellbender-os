#include <stdint.h>
#include <kernel/kernel.h>
#include <coresrv/vga.h>

extern uint16_t vga_buffer[];
                    
void _idc_coresrv_vga_putc(__attribute__((unused))uintptr_t retval,
                           unsigned x, unsigned y, unsigned c, unsigned color) {
  if (x < VGA_WIDTH && y < VGA_HEIGHT) {
    vga_buffer[y * VGA_WIDTH + x] = make_vgaentry(c, color);
  }
}
