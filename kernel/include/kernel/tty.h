#ifndef _KERNEL_TTY_H
#define _KERNEL_TTY_H

#include <stddef.h>

void terminal_early_initialize(void);
void terminal_early_finalize(void);
void terminal_early_writestring(const char* data);

void terminal_putchar(char c);
void terminal_write(const char* data, size_t size);
void terminal_writestring(const char* data);
void terminal_scroll();

#endif
