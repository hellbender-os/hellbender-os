#ifndef __HELLBENDER_KERNEL_SCHEDULER_H__
#define __HELLBENDER_KERNEL_SCHEDULER_H__

#include <stdint.h>

struct thread;
struct process;

#define SCHEDULER_STATE_WAITING        0
#define SCHEDULER_STATE_RUNNABLE       1

#define SCHEDULER_PRIORITY_MIN       1
#define SCHEDULER_PRIORITY_IDLE      1
#define SCHEDULER_PRIORITY_LOWEST    6
#define SCHEDULER_PRIORITY_LOW      11
#define SCHEDULER_PRIORITY_NORMAL   16
#define SCHEDULER_PRIORITY_HIGH     21
#define SCHEDULER_PRIORITY_HIGHEST  26
#define SCHEDULER_PRIORITY_REALTIME 31
#define SCHEDULER_PRIORITY_MAX      31

uint64_t scheduler_run(uint64_t ret_address);
void scheduler_wakeup(struct thread* thread);
void scheduler_sleep_conditionally(struct thread* thread, uint64_t* ptr, uint64_t value);

void scheduler_set_process_priority(struct process* process, unsigned priority);
void scheduler_set_thread_priority(struct thread* thread, unsigned priority);


#endif
