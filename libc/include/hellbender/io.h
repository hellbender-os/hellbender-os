#ifndef __HELLBENDER_KERNEL_IO_H__
#define __HELLBENDER_KERNEL_IO_H__

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

static inline void outw(uint16_t port, uint16_t val)
{
    asm volatile ( "outb %1, %2"
                   : "=m"(__force_order)
                   : "a"(val), "Nd"(port) );
}

static inline uint16_t inw(uint16_t port)
{
    uint16_t ret;
    asm volatile ( "inw %2, %1"
                   : "=m"(__force_order), "=a"(ret)
                   : "Nd"(port) );
    return ret;
}

static inline void outl(uint16_t port, uint32_t val)
{
    asm volatile ( "outl %1, %2"
                   : "=m"(__force_order)
                   : "a"(val), "Nd"(port) );
}

static inline uint32_t inl(uint16_t port)
{
    uint32_t ret;
    asm volatile ( "inl %2, %1"
                   : "=m"(__force_order), "=a"(ret)
                   : "Nd"(port) );
    return ret;
}

static inline void iowait()
{
  inb(0x80);
  inb(0x80);
  inb(0x80);
  inb(0x80);
  inb(0x80);
}

#endif
