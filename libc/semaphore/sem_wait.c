#include <semaphore.h>
#include <hellbender.h>

int sem_wait(sem_t *sem) {
  return syscall_sem_wait(sem);
}
