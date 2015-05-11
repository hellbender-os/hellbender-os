#ifndef _KERNEL_KERNEL_H
#define _KERNEL_KERNEL_H

#include <stddef.h>

#include <kernel/module.h>

// forward declare required stuff
typedef struct process process_t;
typedef struct thread thread_t;
typedef struct domain domain_t;
typedef struct semaphore semaphore_t;

extern unsigned long __force_order;
#define BREAK { asm volatile("xchg %%bx, %%bx" : "=m"(__force_order)); }

#define PAGE_SIZE 4096
#define TABLE_SIZE (1024*PAGE_SIZE)

// kernel is loaded into this address.
#define KERNEL_BASE 0
#define KERNEL_OFFSET (KERNEL_BASE + 0x100000)
#define KERNEL_HEAP_TOP    (KERNEL_OFFSET+TABLE_SIZE)

// current thread page table is mapped into this address.
#define THREAD_OFFSET 0x400000
#define THREAD_TSS_STACK_TOP (THREAD_OFFSET + 2*PAGE_SIZE)
#define THREAD_HEAP_BOTTOM (THREAD_OFFSET+16*PAGE_SIZE)
#define THREAD_HEAP_TOP    (THREAD_OFFSET+TABLE_SIZE-256*PAGE_SIZE)

// all applications are mapped into this address, in their own address spaces.
#define APPLICATION_OFFSET 0x800000

// core module is mapped at a fixed address at the top of the address space.
#define CORE_OFFSET 0x7F800000

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
  // modules loaded by GRUB:
  //unsigned nof_modules;
  //module_binary_t binaries[MAX_MODULES]; // physical address range.
  //kernel_module_t modules[MAX_MODULES]; // virtual address ranges.
  unsigned nof_processes;
  process_t* processes[MAX_MODULES]; // domain and thread.
  unsigned core_module; // index of the core module.

  void* early_data; // holds multiboot information during initialization.
  void* heap_bottom; // heap limits for libK (before processes are set up).
  void* heap_limit;
  int up_and_running; // 0 until initialization done.

  thread_t* current_thread;
  domain_t* current_domain;

  semaphore_t* semaphores;
} kernel_t;

extern kernel_t kernel;

// kernel stack is proceted by two unmapped pages.
// actual top address is kernel_stack + kernel_stack_size.
extern uint8_t kernel_stack[];
#define KERNEL_STACK_SIZE 16384
#define KERNEL_STACK_BOTTOM (kernel_stack + PAGE_SIZE)
#define KERNEL_STACK_TOP (kernel_stack + KERNEL_STACK_SIZE + PAGE_SIZE)

static inline void* ceil_page(void* address) {
  uintptr_t tmp = (uintptr_t)address;
  if (tmp % PAGE_SIZE) tmp += PAGE_SIZE - tmp % PAGE_SIZE;
  return (void*)tmp;
}

__attribute__((__noreturn__))
void kernel_to_usermode();

__attribute__((__noreturn__))
void kernel_halt();

void kernel_main();

#endif
