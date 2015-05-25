#include <kernel/syscall.h>

void syscall_yield() {
  scheduler_goto_next();
}

