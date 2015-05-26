#include <semaphore.h>
#include <coresrv/semaphore.h>
#include <kernel/syscall.h>

int sem_post(sem_t *sem) {
  if (sem->named_id) {
    return CORE_IDC(semaphore_post, sem);
  } else {
    __sync_fetch_and_add(&sem->count, 1);
    syscall_notify(&sem->count);
    return 0;
  }
}
