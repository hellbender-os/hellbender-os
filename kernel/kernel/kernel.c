#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <kernel/kernel.h>
#include <kernel/kstdio.h>
#include <kernel/kstdlib.h>
#include <kernel/idt.h>

kernel_t kernel;

#define KERNEL_STACK_SIZE 16384
uint8_t kernel_stack[KERNEL_STACK_SIZE] __attribute__((aligned(PAGE_SIZE)));
const unsigned kernel_stack_size = KERNEL_STACK_SIZE;

kernel_t* kernel_ptr() {
  return &kernel;
}

void kernel_int50() {
  kprintf("Hello, interrupt World!\n");
}

void kernel_init_interrupts() {
  idt_initialize();

  idt_entry_t int50 = (idt_entry_t) {
    .offset = (uint32_t)&kernel_int50,
    .type = IDT_TYPE_INTERRUPT_32,
    .dpl = 0
  };

  idt_set_entry(0x50, int50);
  idt_enable_interrupts();
}

void kernel_main(void) {
#ifdef DEBUG
  kprintf("kernel_main\n");
#endif

  kernel_init_interrupts();
  
  kprintf("Hello, kernel World!\n");
  idt_send_interrupt(0x50);
  kabort();
}
