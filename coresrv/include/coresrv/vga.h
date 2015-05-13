#ifndef _CORESRV_VGA_H
#define _CORESRV_VGA_H

#include <stddef.h>
#include <kernel/vga.h>

extern void coresrv_vga_putc(unsigned x, unsigned y,
                             unsigned c, unsigned color);

#endif
