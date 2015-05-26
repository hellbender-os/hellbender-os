#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sched.h>

#include <kernel/kernel.h>
#include <kernel/process.h>
#include <kernel/thread.h>
#include <kernel/scheduler.h>

extern void kernel_enter_ring0(uint32_t data_selector,
                               uint32_t stack_address,
                               uint32_t code_selector,
                               uint32_t code_address);

extern void kernel_enter_ring3(uint32_t data_selector,
                               uint32_t stack_address,
                               uint32_t code_selector,
                               uint32_t code_address);

extern void kernel_restore_context();

kernel_t kernel;

uint8_t kernel_stack[KERNEL_STACK_SIZE+2*PAGE_SIZE] __attribute__((aligned(PAGE_SIZE)));

__attribute__((__noreturn__))
void kernel_to_usermode() {
  CURRENT_THREAD->time_start = rdtsc();
  if (CURRENT_THREAD->state == THREAD_STATE_NEW) {
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
