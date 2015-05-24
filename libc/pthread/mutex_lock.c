#include <pthread.h>
#include <hellbender.h>

int pthread_mutex_lock(pthread_mutex_t *mutex) {
  return syscall_mutex_lock(mutex);
}

