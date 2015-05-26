#include <pthread.h>

int pthread_cond_init(pthread_cond_t *cond,
                      const pthread_condattr_t *attr) {
  (void)(attr);//TODO take attr into account
  *cond = PTHREAD_COND_INITIALIZER;
  return 0;
}
