#include <kernel/syscall.h>
#include <kernel/thread.h>
#include <kernel/scheduler.h>

void __attribute__((__noreturn__)) syscall_exit(int status) {
  (void)(status);//TODO record this somewhere
  //printf("Exit: %x\n", (unsigned)status);
  CURRENT_THREAD->state = THREAD_STATE_DEAD;
  scheduler_goto_next();
}
