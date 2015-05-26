#include <pthread.h>
#include <hellbender.h>

int pthread_mutex_init(pthread_mutex_t *mutex,
                       const pthread_mutexattr_t *attr) {
  (void)(attr); // TODO: take attr into account.
  *mutex = PTHREAD_MUTEX_INITIALIZER;
  return 0;
}
