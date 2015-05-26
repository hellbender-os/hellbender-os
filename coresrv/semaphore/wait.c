#include <semaphore.h>
#include <coresrv/semaphore.h>

__IDCIMPL__ int semaphore_wait(IDC_PTR, sem_t* s) {
  semaphore_t *sem = (semaphore_t*)s->named_id;
  return sem_wait(&sem->sem);
}
