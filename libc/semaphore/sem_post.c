#include <semaphore.h>
#include <hellbender.h>

int sem_post(sem_t *sem) {
  return syscall_sem_post(sem);
}
