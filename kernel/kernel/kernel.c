#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <kernel/kernel.h>
#include <kernel/kstdio.h>
#include <kernel/kstdlib.h>
#include <kernel/idt.h>
#include <kernel/isr.h>
#include <kernel/domain.h>

kernel_t kernel;
tss_entry_t kernel_tss;

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

  kernel_init_interrupts();
  
  kprintf("Hello, kernel World!\n");

  // core service has been mapped to the DS and CS by kernel_early.
  // it is a special formatter binary, where entry point is at the beginning,
  // and it includes a special module_t header at offset 8.
  domain_t *core = domain_create_existing((void*)CORE_OFFSET,  // table address
                                          (void*)CORE_OFFSET); // start address
  domain_enter_ring3(core);

  khalt();
}
