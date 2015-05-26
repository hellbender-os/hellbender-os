#include <semaphore.h>

#include <coresrv/semaphore.h>

__IDCIMPL__ int semaphore_post(IDC_PTR, sem_t* s) {
  semaphore_t *sem = (semaphore_t*)s->named_id;
  sem_post(&sem->sem);
  return 0;
}
