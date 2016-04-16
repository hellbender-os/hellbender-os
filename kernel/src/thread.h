#ifndef __HELLBENDER_KERNEL_THREAD_H__
#define __HELLBENDER_KERNEL_THREAD_H__

#include <stdint.h>

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
  void* stack_top;
  uint64_t rsp_backup; // stores the rsp value when thread is not currently being executed.
};

#endif
