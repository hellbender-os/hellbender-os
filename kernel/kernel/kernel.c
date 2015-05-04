#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <kernel/kernel.h>
#include <kernel/kstdio.h>
#include <kernel/kstdlib.h>
#include <kernel/idt.h>
#include <kernel/isr.h>
#include <kernel/io.h>

kernel_t kernel;

#define KERNEL_STACK_SIZE 16384
uint8_t kernel_stack[KERNEL_STACK_SIZE] __attribute__((aligned(PAGE_SIZE)));
const unsigned kernel_stack_size = KERNEL_STACK_SIZE;

kernel_t* kernel_ptr() {
  return &kernel;
}

void kernel_init_interrupts() {
  idt_initialize();
  isr_initialize();
  isr_pic_enable(32);
  isr_pic_enable(33);

  idt_enable_interrupts();
}

void kernel_main(void) {
#ifdef DEBUG
  kprintf("kernel_main\n");
#endif

  kernel_init_interrupts();
  
  kprintf("Hello, kernel World!\n");
  isr_interrupt(80);
  khalt();
  kprintf("kernel_main DONE!\n");
}
