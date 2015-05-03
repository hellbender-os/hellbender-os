#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <kernel/kernel.h>
#include <kernel/vga.h>
#include <kernel/tty.h>
#include <kernel/mem.h>

size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer;
uint8_t terminal_mapped_vga[PAGE_SIZE] __attribute__((aligned(PAGE_SIZE)));

void terminal_early_initialize(void)
{
  // direct-map also the VGA memory so that TTY works when paging is enabled.
  mem_early_map_page((void*)VGA_MEMORY, (uintptr_t)VGA_MEMORY, 3);
  
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

void terminal_early_finalize(void) {
  // we move VGA buffer from its physical location into kernel data area.
  mem_early_map_page(terminal_mapped_vga, (uintptr_t)VGA_MEMORY, 3);
  mem_early_map_page((void*)VGA_MEMORY, 0, 0);
  terminal_buffer = (uint16_t*)&terminal_mapped_vga;
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
    if ( ++terminal_column == VGA_WIDTH )
      {
	terminal_column = 0;
	if ( ++terminal_row == VGA_HEIGHT )
	  {
	    terminal_row = 0;
	  }
      }
  }
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
