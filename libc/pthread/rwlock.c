#include <pthread.h>

// Weak no-op implementations in case pthread lib is not used.

int __attribute__ ((weak)) pthread_rwlock_rdlock(pthread_rwlock_t *rwlock) {
  (void)rwlock;
  return 0;
}

int __attribute__ ((weak)) pthread_rwlock_wrlock(pthread_rwlock_t *rwlock) {
  (void)rwlock;
  return 0;
}

int __attribute__ ((weak)) pthread_rwlock_unlock(pthread_rwlock_t *rwlock) {
  (void)rwlock;
  return 0;
}
