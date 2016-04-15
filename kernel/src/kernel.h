#ifndef __HELLBENDER_KERNEL_H__
#define __HELLBENDER_KERNEL_H__

#include "config.h"
#include "cpu.h"

#include <stdint.h>

INLINE uintptr_t kernel_v2p(void* virtual) { return ((uintptr_t)virtual) & KERNEL_VMASK; }
INLINE void* kernel_p2v(uintptr_t physical) { return (void*)(physical + KERNEL_OFFSET); }

__attribute__((__noreturn__))
void kernel_panic();

void kernel_add_cpu(struct cpu* cpu);

void kernel_main();

#endif
