#include <kernel/syscall.h>
#include <kernel/kernel.h>
#include <kernel/thread.h>
#include <kernel/scheduler.h>

void syscall_wait_neq(volatile unsigned *obj, unsigned value) {
  if (*obj == value) {
    CURRENT_THREAD->state = THREAD_STATE_WAIT;
    CURRENT_THREAD->wait_state =
      (wait_state_t){ .obj = obj, .value = value, .op = THREAD_WAIT_NEQ };
    scheduler_make_wait(CURRENT_THREAD);
    scheduler_goto_next();
  }
}
