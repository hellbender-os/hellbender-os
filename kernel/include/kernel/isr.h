#ifndef _KERNEL_ISR_H
#define _KERNEL_ISR_H

#include <stddef.h>

extern unsigned long __force_order;
#define isr_interrupt(interrupt) \
  asm volatile("int $" #interrupt : "=m"(__force_order));

void isr_initialize();

void isr_pic_eoi(unsigned interrupt);
int isr_pic_check_39();
void isr_pic_enable(unsigned interrupt);
void isr_pic_disable(unsigned interrupt);

#endif
