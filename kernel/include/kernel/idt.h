#ifndef _KERNEL_IDT_H
#define _KERNEL_IDT_H

#include <stddef.h>

typedef struct idt_entry {
  uint32_t offset;
  uint8_t type;
  uint8_t dpl;
} idt_entry_t;

#define IDT_TYPE_TASK_32 5
#define IDT_TYPE_INTERRUPT_16 6
#define IDT_TYPE_TRAP_16 7
#define IDT_TYPE_INTERRUPT_32 14
#define IDT_TYPE_TRAP_32 15

void idt_initialize();
void idt_set_entry(unsigned interrupt, idt_entry_t source);
void idt_enable_interrupts();
void idt_send_interrupt(unsigned interrupt);

#endif
