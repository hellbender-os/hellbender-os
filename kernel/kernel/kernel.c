#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <kernel/kernel.h>
#include <kernel/kstdio.h>
#include <kernel/kstdlib.h>
#include <kernel/idt.h>
#include <kernel/isr.h>
#include <kernel/vmem.h>
#include <kernel/domain.h>
#include <kernel/module.h>
#include <kernel/thread.h>

typedef struct kernel {
} kernel_t;

kernel_t kernel;

uint8_t kernel_stack[KERNEL_STACK_SIZE+2*PAGE_SIZE] __attribute__((aligned(PAGE_SIZE)));

void kernel_init_interrupts() {
  idt_initialize();
  isr_initialize();
  //isr_pic_enable(32); // timer0
  isr_pic_enable(33); // keyboard

  idt_enable_interrupts();
}

void kernel_main(void) {
#ifdef DEBUG
  kprintf("kernel_main\n");
#endif

  vmem_initialize();
  kernel_init_interrupts();
  
  kprintf("Hello, kernel World!\n");

  // core service has been mapped to the DS and CS by kernel_early.
  // it is a special formatter binary, where entry point is at the beginning,
  // and it includes a special module_t header at offset 8.
  domain_t *core = domain_allocate_module((void*)CORE_OFFSET);
  /*thread_t *thread =*/ thread_allocate(core->start);

  kernel_enter_ring3(SEL_USER_DATA|3, (uint32_t)(THREAD_OFFSET + TABLE_SIZE),
                     SEL_USER_CODE|3, (uint32_t)CURRENT_THREAD->start_address);
  __builtin_unreachable();
}
