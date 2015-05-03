#ifndef _KERNEL_KERNEL_H
#define _KERNEL_KERNEL_H

#include <stddef.h>

#define PAGE_SIZE 4096

#define KERNEL_ADDRESS 0x1000
#define CS_BASE 0x80000000

#define SEL_NULL        0x00
#define SEL_KERNEL_CODE 0x08
#define SEL_KERNEL_DATA 0x10
#define SEL_USER_CODE   0x18
#define SEL_USER_DATA   0x20
#define SEL_ALL_DATA    0x28
#define SEL_LAST        0x28
#define SEL_COUNT       ((SEL_LAST/8)+1)

// The first page at CS_BASE
typedef struct kernel {
} kernel_t;

kernel_t* kernel_ptr();

#endif
