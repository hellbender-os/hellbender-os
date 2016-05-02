#ifndef __HELLBENDER_KERNEL_SIGNAL_H__
#define __HELLBENDER_KERNEL_SIGNAL_H__

#include "config.h"

#include <hellbender/list.h>

#include <stdint.h>

struct process;
struct thread;

struct signal_item {
  list_item_t item;
  int signum;
  intptr_t sigvalue;
};

void signal_set_mask(struct thread* thread, uint64_t blocked_mask);

void signal_process(struct process* process, int signum, intptr_t sigval);

void signal_thread(struct thread* thread, int signum, intptr_t sigval);

int signal_wait(struct thread* thread, uint64_t wait_mask);

void signal_delivery();

#endif
