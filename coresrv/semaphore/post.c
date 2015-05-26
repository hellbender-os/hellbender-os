#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>

#include <coresrv/semaphore.h>
#include <kernel/syscall.h>

__IDCIMPL__ int semaphore_post(IDC_PTR, sem_t* s) {
  semaphore_t *sem = (semaphore_t*)s->id;
  __sync_fetch_and_add(&sem->count, 1);
  syscall_notify(&sem->count);
  return 0;
}
