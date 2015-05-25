#include <semaphore.h>
#include <coresrv/semaphore.h>

int sem_post(sem_t *sem) {
  return CORE_IDC(semaphore_post, sem);
}
