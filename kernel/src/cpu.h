#ifndef __HELLBENDER_KERNEL_CPU_H__
#define __HELLBENDER_KERNEL_CPU_H__

#include "thread.h"
#include "tss.h"
#include "heap.h"

#include <stdint.h>

struct cpu {
  union {
    struct {
      uint64_t nested_isr;           // -1 when in ring3; >=0 when in ring0.
      void* kernel_stack_top;        // what stack to use when in ring0.
      
      struct process* current_process; // what is being executed: cpu_set_process
      struct thread* current_thread;   // what is being executed: cpu_set_thread
      uint64_t current_thread_rsp;     // thread_state for interrupt service routines.
      
      tss_t tss;   // per-core TSS.
      heap_t heap; // per-core heap.
    };
    struct __attribute__ ((packed)) {
      uint64_t dummy[511];
      uint64_t thread_local_pt; // thread local pages accessed using recursive mapping.
    };
  };
};

extern struct cpu cpu;

#define CPU_THREAD_STATE ((struct thread_state*)(cpu.current_thread_rsp))

void cpu_enable_features();

void cpu_init();

void cpu_set_process(struct process* process);
void cpu_set_thread(struct thread* thread);

#endif
