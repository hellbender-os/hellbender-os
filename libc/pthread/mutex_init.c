#include <pthread.h>
#include <hellbender.h>

int pthread_mutex_init(pthread_mutex_t *mutex,
                       const pthread_mutexattr_t *attr) {
  return syscall_mutex_init(mutex, attr);
}
