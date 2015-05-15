#ifndef _CORESRV_VGA_H
#define _CORESRV_VGA_H

#include <stddef.h>
#include <kernel/vga.h>
#include <coresrv/coresrv.h>

__IDC__ void coresrv_vga_putc(IDC_PTR, unsigned x, unsigned y, unsigned cursorx, unsigned c, unsigned color);

#endif
