#ifndef __HELLBENDER_KERNEL_CPU_H__
#define __HELLBENDER_KERNEL_CPU_H__

#include "thread.h"
#include "tss.h"
#include "heap.h"

#include <stdint.h>

struct cpu {
  uint64_t nested_isr;           // -1 when in ring3; >=0 when in ring0.
  void* kernel_stack_top;        // what stack to use when in ring0.

  struct thread* current_thread; // what is being executed right now: cpu_set_current(thread);
  uint64_t current_thread_rsp;   // thread_state for interrupt service routines.

  tss_t tss;   // per-core TSS.
  heap_t heap; // per-core heap.
};

extern struct cpu cpu;

void cpu_enable_features();

void cpu_init();

void cpu_set_current(struct thread* thread);

#endif
