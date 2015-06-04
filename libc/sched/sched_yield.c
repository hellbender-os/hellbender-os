#include <sched.h>
#include <hellbender/hellbender.h>

int sched_yield(void) {
  syscall_yield();
  return 0;
}

