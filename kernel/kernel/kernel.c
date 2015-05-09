#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <kernel/kernel.h>
#include <kernel/kstdio.h>
#include <kernel/kstdlib.h>
#include <kernel/idt.h>
#include <kernel/isr.h>
#include <kernel/pit.h>
#include <kernel/pic_isr.h>
#include <kernel/vmem.h>
#include <kernel/domain.h>
#include <kernel/module.h>
#include <kernel/thread.h>
#include <kernel/scheduler.h>

extern void kernel_enter_ring3(uint32_t data_selector,
                               uint32_t stack_address,
                               uint32_t code_selector,
                               uint32_t code_address);

extern void kernel_return_ring3();

kernel_t kernel;

uint8_t kernel_stack[KERNEL_STACK_SIZE+2*PAGE_SIZE] __attribute__((aligned(PAGE_SIZE)));

void kernel_init_interrupts() {
  pit_initialize();
  idt_initialize();
  isr_initialize();
  isr_pic_enable(32); // timer0
  isr_pic_enable(33); // keyboard
}

void kernel_main(void) {
#ifdef DEBUG
  kprintf("kernel_main\n");
#endif

  vmem_initialize();
  scheduler_initialize();
  kernel_init_interrupts();
  
  kprintf("Hello, kernel World!\n");

  // core service has been mapped to the DS and CS by kernel_early.
  domain_t *core = domain_allocate_module((void*)CORE_OFFSET);
  thread_t *sys1_thread = thread_allocate(NULL);
  thread_t *sys2_thread = thread_allocate(NULL);
  thread_t *usr1_thread = thread_allocate(core->start);
  thread_t *usr2_thread = thread_allocate(core->start);

  thread_set_current(sys1_thread); // so that something is mapped in the memory.
  core_service_t *core_info = (core_service_t*)(CORE_MODULE->module_info);
  pic_isr_initialize(sys1_thread, sys2_thread, core_info);
  idt_enable_interrupts();
  scheduler_set_realtime_thread(sys1_thread);
  scheduler_add_thread(sys2_thread);
  scheduler_add_thread(usr1_thread);
  scheduler_add_thread(usr2_thread);
  scheduler_goto_next();
}

__attribute__((__noreturn__))
void kernel_to_usermode() {
  if (CURRENT_THREAD->state == THREAD_STATE_NEW) {
    CURRENT_THREAD->state = THREAD_STATE_OLD;
    kernel_enter_ring3(SEL_USER_DATA|3,
                       (uint32_t)(THREAD_OFFSET + TABLE_SIZE),
                       SEL_USER_CODE|3,
                       (uint32_t)CURRENT_THREAD->start_address);
  } else {
    kernel_return_ring3();
  }
  __builtin_unreachable();
}
