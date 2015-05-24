#include <pthread.h>
#include <hellbender.h>

int pthread_cond_init(pthread_cond_t *cond,
                      const pthread_condattr_t *attr) {
  return syscall_cond_init(cond, attr);
}
