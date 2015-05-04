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

// kernel stack is proceted by two unmapped pages.
// actual top address is kernel_stack + kernel_stack_size.
#define KERNEL_STACK_SIZE 16384
uint8_t kernel_stack[KERNEL_STACK_SIZE+2*PAGE_SIZE] __attribute__((aligned(PAGE_SIZE)));
const unsigned kernel_stack_size = KERNEL_STACK_SIZE+PAGE_SIZE;

void kernel_init_interrupts() {
  idt_initialize();
  isr_initialize();
  //isr_pic_enable(32); // timer0
  isr_pic_enable(33); // keyboard

  idt_enable_interrupts();
}

void kernel_in_ring3() {
  isr_interrupt(80);
  while (1) {}
}

extern void kernel_enter_ring3(uint32_t data_selector,
                               uint32_t stack_address,
                               uint32_t code_selector,
                               uint32_t code_address);

void kernel_main(void) {
#ifdef DEBUG
  kprintf("kernel_main\n");
#endif

  kernel_init_interrupts();
  
  kprintf("Hello, kernel World!\n");

  domain_t *userspace = domain_create();
  void *userspace_data = domain_alloc_data(userspace, PAGE_SIZE);
  void *userspace_code = domain_alloc_code(userspace, PAGE_SIZE);
  kprintf("domain.data = %x; domain.code = %x\n",
          (unsigned)userspace_data, (unsigned)userspace_code);
  
  kernel_enter_ring3(SEL_USER_DATA|3, (uint32_t)(KERNEL_STACK_TOP),
                     SEL_USER_CODE|3, (uint32_t)&kernel_in_ring3);
  
  khalt();
  kprintf("kernel_main DONE!\n");
}
