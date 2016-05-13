#ifndef __HELLBENDER_KERNEL_THREAD_H__
#define __HELLBENDER_KERNEL_THREAD_H__

#include "spin.h"
#include "pqueue.h"
#include "signal.h"

#include <hellbender/list.h>
#include <hellbender/fifo.h>

#include <stdint.h>
#include <stddef.h>

struct process;
struct libc_init;

struct thread_context {
  uint64_t error, rip, cs, rflags, rsp, ss;
} __attribute__ ((packed));

struct thread_registers {
  uint64_t rax, rbx, rcx, rdx, rbp, rsi, rdi;
  uint64_t r8, r9, r10, r11, r12, r13, r14, r15;
  uint64_t tmp; // for stack alignment & tmp storage in isr_asm.S.
} __attribute__ ((packed));

struct thread_state {
  struct thread_registers registers;
  struct thread_context context;
} __attribute__ ((packed));

struct thread {
  union {
    struct {
      spinlock_raw_t lock;

      list_item_t process_threads; // process->threads list.

      uint64_t tid;
      struct process *process;
      void* stack_top;     // TSS stack to store registers on interrupt.
      uint64_t rsp_backup; // stores the rsp value when thread is not currently being executed.
      uint64_t ret_address; // address from which scheduler_run was called.
      uint64_t *thread_local_pt; // specially mapped pages for thread local variables.
      size_t thread_local_pages; // how many pages are in the TL PT.
      unsigned in_service; // 1 if thread is executing in service domain.
    };
    uint8_t dummy[CACHE_LINE];
  };

  union thread_signal {
    struct {
      spinlock_raw_t lock;    // if you need process->signal.lock, you must lock that first!
      uint64_t blocked_mask;  // bit set => signal is blocked.
      uint64_t delivery_mask; // bit set => signal temporarily blocked while running action.
      uint64_t wait_mask;     // bit set => thread is waiting the signal.
      int wait_signum;        // > 0 when the thread received a signal it waited for.
      intptr_t wait_sigval;
      list_item_t waiting_item;// process.signal.waiting
      uint64_t pending_mask;   // bit set => sig[x].pending is not empty.
      struct thread_signal_info {
        fifo_t pending;         // struct signal_pending (in signal.c).
      } sig[SIGNAL_LIMIT];
    };
    uint8_t dummy[CACHE_LINE];
  } signal;

  // these fields are to be touched only by scheduler_* functions in scheduler.c
  struct thread_scheduler {
    unsigned state;              // one of SCHEDULER_STATE_*
    unsigned priority;           // one of SCHEDULER_PRIORITY_*
    pqueue_item_t runnable_item; // process.scheduler.runnable_threads
  } scheduler;
};

struct thread* thread_create(struct process* process, uintptr_t stack_top,
                             uintptr_t entry_point, uintptr_t user_value);

void thread_local_invalidate(size_t pages);

#endif
