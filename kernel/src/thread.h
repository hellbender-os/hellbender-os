#ifndef __HELLBENDER_KERNEL_THREAD_H__
#define __HELLBENDER_KERNEL_THREAD_H__

#include "spin.h"

#include <stdint.h>
#include <stddef.h>

struct process;

struct thread_context {
  uint64_t error, rip, cs, rflags, rsp, ss;
} __attribute__ ((packed));

struct thread_registers {
  uint64_t rax, rbx, rcx, rdx, rbp, rsi, rdi;
  uint64_t r8, r9, r10, r11, r12, r13, r14, r15;
  uint64_t unused; // for stack alignment.
} __attribute__ ((packed));

struct thread_state {
  struct thread_registers registers;
  struct thread_context context;
} __attribute__ ((packed));

struct thread {
  spinlock_raw_t lock;
  uint64_t tid;
  struct process *process;
  void* stack_top;     // TSS stack to store registers on interrupt.
  uint64_t rsp_backup; // stores the rsp value when thread is not currently being executed.
  uint64_t *thread_local_pt; // specially mapped pages for thread local variables.
  size_t thread_local_pages; // how many pages are in the TL PT.
};

struct thread* thread_create(struct process* process,
                             uintptr_t entry_point, uintptr_t stack_top,
                             uintptr_t local_bottom, size_t local_size);

void thread_local_invalidate(size_t pages);

#endif
