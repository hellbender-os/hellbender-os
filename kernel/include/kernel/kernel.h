#ifndef _KERNEL_KERNEL_H
#define _KERNEL_KERNEL_H

#include <stddef.h>

#include <kernel/tss.h>

#define PAGE_SIZE 4096

#define KERNEL_ADDRESS 0x1000
#define CS_BASE 0x80000000

#define SEL_NULL        0x00
#define SEL_KERNEL_CODE 0x08
#define SEL_KERNEL_DATA 0x10
#define SEL_USER_CODE   0x18
#define SEL_USER_DATA   0x20
#define SEL_ALL_DATA    0x28
#define SEL_TSS         0x30
#define SEL_LAST        0x30
#define SEL_COUNT       ((SEL_LAST/8)+1)

// The first page at CS_BASE
typedef struct kernel {
} kernel_t;

extern kernel_t kernel;
extern tss_entry_t kernel_tss;
extern uint8_t kernel_stack[];
extern const unsigned kernel_stack_size;
#define KERNEL_STACK_TOP (kernel_stack+kernel_stack_size)

#endif
