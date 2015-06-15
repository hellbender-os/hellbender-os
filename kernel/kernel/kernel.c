#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <sched.h>

#include <kernel/kernel.h>
#include <kernel/process.h>
#include <kernel/thread.h>
#include <kernel/scheduler.h>

#include <hellbender/signal_wrapper.h>
#include <hellbender/threadlocal.h>

extern void kernel_enter_ring0(uint32_t data_selector,
                               uint32_t stack_address,
                               uint32_t code_selector,
                               uint32_t code_address);

extern void kernel_enter_ring3(uint32_t data_selector,
                               uint32_t stack_address,
                               uint32_t code_selector,
                               uint32_t code_address);

extern void kernel_restore_context();

void kernel_process_signal();

kernel_t kernel;

uint8_t kernel_stack[KERNEL_STACK_SIZE+2*PAGE_SIZE] __attribute__((aligned(PAGE_SIZE)));

__attribute__((__noreturn__))
void kernel_to_usermode() {
  if (CURRENT_THREAD->state == THREAD_STATE_NEW) {
    CURRENT_THREAD->time_start = rdtsc();
    CURRENT_THREAD->state = THREAD_STATE_OLD;
    if (CURRENT_THREAD->is_kernel) {
      kernel_enter_ring0(SEL_KERNEL_DATA,
                         (uint32_t)CURRENT_THREAD->stack_top,
                         SEL_KERNEL_CODE,
                         (uint32_t)CURRENT_THREAD->start_address);
    } else {
      kernel_enter_ring3(SEL_USER_DATA|3,
                         (uint32_t)CURRENT_THREAD->stack_top,
                         SEL_USER_CODE|3,
                         (uint32_t)CURRENT_THREAD->start_address);
    }
  } else {
    if (THREADLOCAL->signal_pending & THREADLOCAL->signal_mask) {
      kernel_process_signal();
    }
    CURRENT_THREAD->time_start = rdtsc();
    kernel_restore_context();
  }
  __builtin_unreachable();
}

__attribute__((__noreturn__))
void kernel_halt(void) {
  // we switch the current thread before enabling interrupts, otherwise
  // we'd screw up thread state on the next interrupt (we are in ring 0).
  // ISRs should never return, so this thread will not get executed.
  // TODO: should we make this thread "idle" in some infinite loop?
  thread_set_current(kernel.processes[0]->thread);
  while (1) { asm("sti; hlt"); }
  __builtin_unreachable();
}

// Make the current thread call the signal handler.
void kernel_process_signal() {
  // TODO: add default actions!
  
  // find the higest priority signal to process:
  uint32_t sigs = THREADLOCAL->signal_pending & THREADLOCAL->signal_mask;
  int signum  = ffs((int)sigs);
  if (!signum || !THREADLOCAL->signal_wrapper) return;
  THREADLOCAL->signal_pending &= ~(1 << (signum-1));

  // setup stacks so that the thread will enter the signal_wrapper:
  
  uint32_t* kstack = (uint32_t*)CURRENT_THREAD->kernel_stack_ptr;
  uint32_t* ustack = (uint32_t*)kstack[15];
  // kstack contains GS,FS,ES,DS,8 regs,EIP,CS,eflags,ESP,SS
  // copy the first 15 values from kernel stack into user stack.
  // this way signal_wrapper can restore context to return to user code.
  uint32_t* stack = ustack;
  for (int i = 14; i >= 0; --i) {
    *--stack = kstack[i];
  }
  // push signal_wrapper parameters into ustack.
  *--stack = (uint32_t)&THREADLOCAL->signal_actions[signum-1];
  *--stack = (uint32_t)NULL; //TODO: this should be ucontext_t
  *--stack = (uint32_t)NULL; //TODO: this should be siginfo_t
  *--stack = (uint32_t)signum;
  // change the kernel return address to signal_wrapper.
  kstack[12] = (uint32_t)THREADLOCAL->signal_wrapper;
}

