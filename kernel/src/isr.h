#ifndef __HELLBENDER_KERNEL_ISR_H__
#define __HELLBENDER_KERNEL_ISR_H__

struct process;

void isr_set_signal(unsigned irqnum, struct process *process, int signum);
void isr_clear_signal(unsigned irqnum, struct process *process);

// Sends a signal to the process that has enabled the IRQ.
void isr_send_signal(unsigned irqnum);

#endif
