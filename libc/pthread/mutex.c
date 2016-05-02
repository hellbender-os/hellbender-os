#include <pthread.h>

// Weak no-op implementations in case pthread lib is not used.

int __attribute__ ((weak)) pthread_mutex_lock(pthread_mutex_t *mutex) {
  (void)mutex;
  return 0;
}

int __attribute__ ((weak)) pthread_mutex_unlock(pthread_mutex_t *mutex) {
  (void)mutex;
  return 0;
}
