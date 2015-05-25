#include <semaphore.h>
#include <coresrv/semaphore.h>

int sem_wait(sem_t *sem) {
  return CORE_IDC(semaphore_wait, sem);
}
