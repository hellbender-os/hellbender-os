#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <kernel/syscall.h>
#include <hellbender/threadlocal.h>

int pthread_mutex_unlock(pthread_mutex_t *m) {
  if (m->owner != THREADLOCAL->thread_id) {
    printf("Not mutex owner!\n");
    abort();
  }
  __sync_fetch_and_add(&m->count, 1);
  syscall_notify(&m->count);
  return 0;
}
