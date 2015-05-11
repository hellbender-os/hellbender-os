#ifndef _KERNEL_TTY_H
#define _KERNEL_TTY_H

#include <stddef.h>

void terminal_stage_1_init();
void terminal_stage_2_remap();
void terminal_stage_3_cleanup();

 // terminal works on every stage after terminal_stage_1_init has been called.

void terminal_putchar(char c);
void terminal_write(const char* data, size_t size);
void terminal_writestring(const char* data);
void terminal_scroll();

#endif
