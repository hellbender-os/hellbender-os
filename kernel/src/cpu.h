#ifndef __HELLBENDER_KERNEL_CPU_H__
#define __HELLBENDER_KERNEL_CPU_H__

#include "thread.h"
#include "heap.h"

#include <stdint.h>

struct cpu {
  uint64_t nested_isr;           // -1 when in ring3; >=0 when in ring0.
  void* kernel_stack_top;        // what stack to user when in ring0.

  struct thread* current_thread; // what is being executed right now: cpu_set_current(thread);
  uint64_t current_thread_rsp;   // thread_state for interrupt service routines.

  heap_t heap;
};

extern struct cpu cpu;

void cpu_init();

/* copy current_thread_rsp both ways,
 * set TSS stack pointer.
 */
void cpu_set_current(struct thread* thread);

#endif
