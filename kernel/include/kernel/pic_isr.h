#ifndef _KERNEL_PIC_ISR
#define _KERNEL_PIC_ISR

void pic_isr_stage_4_init();

__attribute__((__noreturn__))
void pic_isr_process_next();

#endif
