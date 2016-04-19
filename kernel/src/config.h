#ifndef __HELLBENDER_KERNEL_CONFIG_H__
#define __HELLBENDER_KERNEL_CONFIG_H__

#define INLINE __attribute__((always_inline)) inline

#define CACHE_LINE 64

#define PAGE_SIZE 4096
#define PAGE_VIRTUAL_MASK (~(PAGE_SIZE-1))
#define PAGE_PHYSICAL_MASK 0xFFFFFFFFF000
#define PAGE_OFFSET_MASK 0xFFF

#define TABLE_SIZE (512*PAGE_SIZE)
#define TABLE_VIRTUAL_MASK (~(TABLE_SIZE-1))
#define TABLE_PHYSICAL_MASK 0xFFFFFFE00000
#define TABLE_OFFSET_MASK 0x1FFFFF

#define KERNEL_STACK_SIZE (2*PAGE_SIZE)
#define USER_STACK_SIZE   (2*PAGE_SIZE) // TODO: more when we have on demand allocation.

#define N_SELECTORS 5       // excluding the TSS selector
#define NULL_SELECTOR  0x00
#define CODE_SELECTOR  0x08
#define DATA_SELECTOR  0x10
#define UCODE_SELECTOR (0x18|3)
#define UDATA_SELECTOR (0x20|3)
#define TSS_SELECTOR   0x28

#define HIGH_MEMORY_LIMIT (0x10000000)

#define KERNEL_OFFSET (0xFFFF800000000000)
#define KERNEL_VMASK  (    0x7FFFFFFFFFFF)

#define CPU_PAGE_ADDRESS  0xFFFFFFFFFFFFE000 // where the CPU structure is mapped
#define THREAD_LOCAL_BASE 0xFFFFFFFFBFE00000 // where the thread locals are mapped

#define PROCESS_CONTEXT_MASK 0xFFF

#endif
