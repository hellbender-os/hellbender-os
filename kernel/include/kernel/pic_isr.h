#ifndef _KERNEL_PIC_ISR
#define _KERNEL_PIC_ISR

#include <kernel/module.h>

void pic_isr_stage_4_init(core_service_t *core, process_t *core_process);

__attribute__((__noreturn__))
void pic_isr_process_next();

#endif
