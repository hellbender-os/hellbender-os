#include <kernel/syscall.h>
#include <kernel/scheduler.h>

void syscall_yield() {
  scheduler_goto_next();
}

