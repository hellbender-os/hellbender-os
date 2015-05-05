#ifndef _KERNEL_KERNEL_H
#define _KERNEL_KERNEL_H

#include <stddef.h>

#include <kernel/tss.h>

extern unsigned long __force_order;
#define BREAK { asm volatile("xchg %%bx, %%bx" : "=m"(__force_order)); }

#define PAGE_SIZE 4096
#define TABLE_SIZE (1024*PAGE_SIZE)

#define KERNEL_OFFSET 0x100000
#define CORE_OFFSET 0x7F800000
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

typedef struct kernel_module {
  uint32_t bottom;
  uint32_t top;
  uint32_t text_bottom;
  uint32_t text_top;
} __attribute__((packed)) kernel_module_t;

typedef struct kernel {
} kernel_t;

extern kernel_t kernel;
extern tss_entry_t kernel_tss;

// kernel stack is proceted by two unmapped pages.
// actual top address is kernel_stack + kernel_stack_size.
extern uint8_t kernel_stack[];
#define KERNEL_STACK_SIZE 16384
#define KERNEL_STACK_BOTTOM (kernel_stack + PAGE_SIZE)
#define KERNEL_STACK_TOP (kernel_stack + KERNEL_STACK_SIZE + PAGE_SIZE)

extern void kernel_enter_ring3(uint32_t data_selector,
                               uint32_t stack_address,
                               uint32_t code_selector,
                               uint32_t code_address);

static inline void* ceil_page(void* address) {
  return address + (uintptr_t)address % PAGE_SIZE;
}

#endif
