#include <pthread.h>
#include <hellbender.h>

int pthread_mutex_unlock(pthread_mutex_t *mutex) {
  return syscall_mutex_unlock(mutex);
}
