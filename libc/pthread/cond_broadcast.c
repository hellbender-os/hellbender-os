#include <pthread.h>
#include <hellbender.h>

int pthread_cond_broadcast(pthread_cond_t *cond) {
  return syscall_cond_broadcast(cond);
}
