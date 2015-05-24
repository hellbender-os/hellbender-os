#include <pthread.h>
#include <hellbender.h>

int pthread_cond_wait(pthread_cond_t *cond,
                      pthread_mutex_t *mutex) {
  return syscall_cond_wait(cond, mutex);
}
