#ifndef _KERNEL_PIC_ISR
#define _KERNEL_PIC_ISR

#include <kernel/thread.h>
#include <kernel/module.h>

void pic_isr_initialize(thread_t *timer_thread, thread_t *thread,
                        core_service_t *core);

__attribute__((__noreturn__))
void pic_isr_process_next();

#endif
