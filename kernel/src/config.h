#ifndef __HELLBENDER_KERNEL_CONFIG_H__
#define __HELLBENDER_KERNEL_CONFIG_H__

#ifdef __is_hellbender_kernel
#define BREAK asm( "xchgw %bx, %bx" )
#else
#define BREAK xchgw %bx, %bx
#endif

#define INLINE __attribute__((always_inline)) inline

#define CACHE_LINE 64

#define PAGE_SIZE 4096
#define PAGE_ADDRESS_MASK (~(PAGE_SIZE-1))

#define TABLE_SIZE (512*PAGE_SIZE)
#define TABLE_ADDRESS_MASK (~(TABLE_SIZE-1))

#define CPU_STACK_SIZE (2*PAGE_SIZE)

#define N_SELECTORS 5
#define CODE_SELECTOR  0x08
#define DATA_SELECTOR  0x10
#define UCODE_SELECTOR 0x18
#define UDATA_SELECTOR 0x20

#define HIGH_MEMORY_LIMIT (0x10000000)

#define KERNEL_OFFSET (0xFFFF800000000000)
#define KERNEL_VMASK  (    0x7FFFFFFFFFFF)

#endif
