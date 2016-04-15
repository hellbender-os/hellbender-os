#ifndef __HELLBENDER_KERNEL_CPU_H__
#define __HELLBENDER_KERNEL_CPU_H__

#include "heap.h"

#include <stdint.h>

struct cpu {
  heap_t heap;
};

extern struct cpu cpu;

void cpu_init();

#endif
