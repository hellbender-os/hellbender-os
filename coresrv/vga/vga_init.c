#include <string.h>
#include <kernel/kernel.h>

// we need one page where vga buffer will be mapped into.
uint16_t vga_buffer[PAGE_SIZE / 2] __attribute__((aligned(PAGE_SIZE)));

void vga_init() {
  //memset(vga_buffer, 0, PAGE_SIZE);
}
