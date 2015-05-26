#include <pthread.h>
#include <kernel/syscall.h>

int pthread_cond_wait(pthread_cond_t *cond,
                      pthread_mutex_t *mutex) {
  // FIFO: each thread gets a sequence number, only the thread with
  // the smallest sequence number may exit.
  unsigned seq_nr = __sync_fetch_and_add(&cond->entry_nr, 1);
  __sync_fetch_and_add(&cond->waiting, 1);
  // we release the mutex while we wait for a release.
  pthread_mutex_unlock(mutex);
  // wait until it's our turn to leave.
  if (cond->exit_nr != seq_nr) {
    syscall_wait_eq(&cond->exit_nr, seq_nr);
  }
  // wait until we are good to leave.
  if (cond->released == 0) {
    syscall_wait_neq(&cond->released, 0);
  }
  // acquire the mutex before leaving.
  pthread_mutex_lock(mutex);
  __sync_fetch_and_sub(&cond->released, 1);
  __sync_fetch_and_sub(&cond->waiting, 1);
  __sync_fetch_and_add(&cond->exit_nr, 1);
  syscall_notify(&cond->exit_nr);
  return 0;
}
