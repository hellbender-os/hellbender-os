#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/threadlocal.h>
#include <kernel/syscall.h>

int pthread_mutex_lock(pthread_mutex_t *m) {
  // FIFO: each thread gets a sequence number, only the thread with
  // the smallest sequence number may exit.
  unsigned seq_nr = __sync_fetch_and_add(&m->entry_nr, 1);
  if (m->exit_nr != seq_nr) {
    // TODO: protect for priority inversion!
    syscall_wait_eq(&m->exit_nr, seq_nr);
  }
  if (m->count == 0) {
    // TODO: protect for priority inversion!
    syscall_wait_neq(&m->count, 0);
  }
  m->owner = THREADLOCAL->thread_id;
  __sync_fetch_and_sub(&m->count, 1);
  __sync_fetch_and_add(&m->exit_nr, 1);
  syscall_notify(&m->exit_nr);
  return 0;
}

