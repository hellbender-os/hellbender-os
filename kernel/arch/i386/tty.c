#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <kernel/kernel.h>
#include <kernel/vga.h>
#include <kernel/tty.h>
#include <kernel/mmap.h>
#include <kernel/mem.h>
#include <kernel/io.h>

size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer;

// We want to re-map the VGA buffer above 1MB, so we use this virtual space.
// The two pages are guard pages that will be unmapped.
uint8_t terminal_mapped_vga[3*PAGE_SIZE] __attribute__((aligned(PAGE_SIZE)));

void terminal_stage_1_init() {
  terminal_row = 0;
  terminal_column = 0;
  terminal_color = make_color(COLOR_LIGHT_GREY, COLOR_BLACK);
  terminal_buffer = VGA_MEMORY;
  for ( size_t y = 0; y < VGA_HEIGHT; y++ )
    {
      for ( size_t x = 0; x < VGA_WIDTH; x++ )
	{
	  const size_t index = y * VGA_WIDTH + x;
	  terminal_buffer[index] = make_vgaentry(' ', terminal_color);
	}
    }
}

void terminal_stage_2_remap() {
  // direct-map the VGA memory so that TTY works when paging is enabled.
  mmap_stage_2_map_page((void*)VGA_MEMORY, (uintptr_t)VGA_MEMORY,
                        MMAP_ATTRIB_KERNEL_RW);
  
}

void terminal_stage_3_cleanup() {
  // we move VGA buffer from its physical location into kernel data area.

  // make the underlying RAM avaiable again (unmap also the guard pages).
  mem_free_page(mmap_unmap_page(terminal_mapped_vga));
  mem_free_page(mmap_unmap_page(terminal_mapped_vga + 1*PAGE_SIZE));
  mem_free_page(mmap_unmap_page(terminal_mapped_vga + 2*PAGE_SIZE));

  // then just move the terminal memory area.
  mmap_map_page(terminal_mapped_vga + PAGE_SIZE, (uintptr_t)VGA_MEMORY,
                MMAP_ATTRIB_KERNEL_RW);
  mmap_unmap_page((void*)VGA_MEMORY);

  terminal_buffer = (uint16_t*)(terminal_mapped_vga + PAGE_SIZE);
}

void terminal_setcolor(uint8_t color)
{
  terminal_color = color;
}

void terminal_putentryat(char c, uint8_t color, size_t x, size_t y)
{
  const size_t index = y * VGA_WIDTH + x;
  terminal_buffer[index] = make_vgaentry(c, color);
}

void terminal_putchar(char c)
{
  if (c == '\n') {
    terminal_column = 0;
    if (terminal_row < VGA_HEIGHT-1) {
      ++terminal_row;
    } else {
      terminal_scroll();
    }
  } else {
    terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
    if ( ++terminal_column == VGA_WIDTH ) {
      terminal_column = 0;
      if (terminal_row < VGA_HEIGHT-1) {
        ++terminal_row;
      } else {
        terminal_scroll();
      }
    }
  }

  unsigned position = terminal_row * VGA_WIDTH + terminal_column;
  // cursor LOW port to vga INDEX register
  outb(0x3D4, 0x0F);
  outb(0x3D5, (unsigned char)(position&0xFF));
  // cursor HIGH port to vga INDEX register
  outb(0x3D4, 0x0E);
  outb(0x3D5, (unsigned char )((position>>8)&0xFF));
}

void terminal_write(const char* data, size_t size)
{
  for ( size_t i = 0; i < size; i++ )
    terminal_putchar(data[i]);
}

void terminal_writestring(const char* data)
{
  terminal_write(data, strlen(data));
}

void terminal_scroll() {
  for ( size_t y = 1; y < VGA_HEIGHT; y++ ) {
    for ( size_t x = 0; x < VGA_WIDTH; x++ ) {
      const size_t dst = (y-1) * VGA_WIDTH + x;
      const size_t src = y * VGA_WIDTH + x;
      terminal_buffer[dst] = terminal_buffer[src];
    }
  }
  for ( size_t x = 0; x < VGA_WIDTH; x++ ) {
    const size_t dst = (VGA_HEIGHT-1) * VGA_WIDTH + x;
    terminal_buffer[dst] = make_vgaentry(' ', terminal_color);
  }
}
