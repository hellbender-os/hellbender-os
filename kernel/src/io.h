#ifndef __HELLBENDER_KERNEL_IO_H__
#define __HELLBENDER_KERNEL_IO_H__

#include "kernel.h"
#include <stdint.h>

INLINE void outb(uint16_t port, uint8_t val) {
    asm volatile ( "outb %1, %2"
                   : "=m"(__force_order)
                   : "a"(val), "Nd"(port) );
}

INLINE uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile ( "inb %2, %1"
                   : "=m"(__force_order), "=a"(ret)
                   : "Nd"(port) );
    return ret;
}

INLINE void iowait() {
  inb(0x80);
  inb(0x80);
  inb(0x80);
  inb(0x80);
  inb(0x80);
}

#endif
