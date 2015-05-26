#include <semaphore.h>
#include <coresrv/semaphore.h>
#include <kernel/syscall.h>

int sem_wait(sem_t *sem) {
  if (sem->named_id) {
    return CORE_IDC(semaphore_wait, sem);
  } else {
    // FIFO: each thread gets a sequence number, only the thread with
    // the smallest sequence number may exit.
    unsigned seq_nr = __sync_fetch_and_add(&sem->entry_nr, 1);
    if (sem->exit_nr != seq_nr) {
      syscall_wait_eq(&sem->exit_nr, seq_nr);
    }
    if (sem->count == 0) {
      syscall_wait_neq(&sem->count, 0);
    }
    __sync_fetch_and_sub(&sem->count, 1);
    __sync_fetch_and_add(&sem->exit_nr, 1);
    syscall_notify(&sem->exit_nr);
    return 0;
  }
}
