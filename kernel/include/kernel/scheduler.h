#ifndef _KERNEL_SCHEDULER_H
#define _KERNEL_SCHEDULER_H

#include <kernel/thread.h>

void scheduler_initialize();
void scheduler_set_realtime_thread(thread_t *thread);
void scheduler_add_thread(thread_t *thread);

void scheduler_make_wait(thread_t *thread);

__attribute__((__noreturn__))
void scheduler_goto_next();

#endif
