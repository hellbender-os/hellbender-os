#ifndef _KERNEL_IO_H
#define _KERNEL_IO_H

#include <stdint.h>

extern unsigned long __force_order;

static inline void outb(uint16_t port, uint8_t val)
{
    asm volatile ( "outb %1, %2"
                   : "=m"(__force_order)
                   : "a"(val), "Nd"(port) );
}

static inline uint8_t inb(uint16_t port)
{
    uint8_t ret;
    asm volatile ( "inb %2, %1"
                   : "=m"(__force_order), "=a"(ret)
                   : "Nd"(port) );
    return ret;
}

#endif
