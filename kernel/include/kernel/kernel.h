#ifndef _KERNEL_KERNEL_H
#define _KERNEL_KERNEL_H

#include <stddef.h>

#include <kernel/module.h>
#include <kernel/tss.h>

extern unsigned long __force_order;
#define BREAK { asm volatile("xchg %%bx, %%bx" : "=m"(__force_order)); }

#define PAGE_SIZE 4096
#define TABLE_SIZE (1024*PAGE_SIZE)

#define KERNEL_OFFSET 0x100000 // kernel is loaded into this address.

// virtual address space is split into two segments to simulate 'no-execute'
// flag in protected mode.
// we don't need this when we switch to 64-bit long mode.
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

// maximum number of modules that can be loaded by GRUB (kernel included).
#define MAX_MODULES 4

typedef struct kernel {
  // text and data address ranges for all modules.
  // the first one is the actual kernel.
  unsigned nof_modules;
  kernel_module_t modules[MAX_MODULES];
} kernel_t;

extern kernel_t kernel;

// kernel stack is proceted by two unmapped pages.
// actual top address is kernel_stack + kernel_stack_size.
extern uint8_t kernel_stack[];
#define KERNEL_STACK_SIZE 16384
#define KERNEL_STACK_BOTTOM (kernel_stack + PAGE_SIZE)
#define KERNEL_STACK_TOP (kernel_stack + KERNEL_STACK_SIZE + PAGE_SIZE)

__attribute__((__noreturn__))
void kernel_to_usermode();

static inline void* ceil_page(void* address) {
  return address + (uintptr_t)address % PAGE_SIZE;
}

#endif
