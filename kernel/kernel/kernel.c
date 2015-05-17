#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sched.h>

#include <kernel/kernel.h>
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

void kernel_main(void) {
  printf("kernel_main\n");
  
  printf("Hello, kernel World!\n");
  exit(0);
}

__attribute__((__noreturn__))
void kernel_to_usermode() {
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
  while (1) { asm("sti; hlt"); }
  __builtin_unreachable();
}
