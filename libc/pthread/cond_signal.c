#include <pthread.h>
#include <kernel/syscall.h>

int pthread_cond_signal(pthread_cond_t *cond) {
  // if we own the mutex others are waiting with, this is totally safe.
  // otherwise we might increase released count above that is being waited for.
  // it means that the next waiting thread will not block, but as per posix:
  // "if predictable scheduling behaviour is required, then that mutex is
  //  locked by the thread calling pthread_cond_signal()".
  if (cond->waiting) {
    __sync_fetch_and_add(&cond->released, 1);
    syscall_notify(&cond->released);
  }
  return 0;
}
