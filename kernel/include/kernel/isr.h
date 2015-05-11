#ifndef _KERNEL_ISR_H
#define _KERNEL_ISR_H

#include <stddef.h>

extern unsigned long __force_order;
#define isr_interrupt(interrupt) \
  asm volatile("int $" #interrupt : "=m"(__force_order));

void isr_stage_1_debugger();
void isr_stage_3_trap();
void isr_stage_3_syscall();
void isr_stage_4_pic();

#endif
