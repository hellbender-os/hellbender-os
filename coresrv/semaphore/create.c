#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>

#include <coresrv/semaphore.h>
#include <kernel/syscall.h>

typedef struct semaphore {
  struct semaphore* next;
  const char* name;
  volatile unsigned count;
  volatile unsigned entry_nr;
  volatile unsigned exit_nr;
} semaphore_t;

static semaphore_t *semaphores;

__IDCIMPL__ int semaphore_create(IDC_PTR, sem_t *sem, const char* name, int oflag, mode_t mode, unsigned value) {
  (void)(oflag);//TODO
  (void)(mode);//TODO
  size_t len = strlen(name) + 1;
  if (len > SEMAPHORE_MAX_NAME) {
    printf("Maximum semaphore length exceeded.\n");
    abort();
  }

  semaphore_t* s = (semaphore_t*)malloc(sizeof(semaphore_t) + len);
  memset(s, 0, sizeof(semaphore_t));
  s->count = value;
  if (len) {
    s->name = ((char*)s) + sizeof(semaphore_t);
    memcpy((char*)s->name, name, len);
    s->next = semaphores;
    semaphores = s;
  }
  sem->id = s;
  return 0;
}

__IDCIMPL__ int semaphore_open(IDC_PTR, sem_t *sem, const char* name, int oflag) {
  (void)(oflag);//TODO
  for (semaphore_t *s = semaphores; s; s = s->next) {
    if (strcmp(s->name, name) == 0) {
      sem->id = s;
      return 0;
    }
  }
  return 1;
}

__IDCIMPL__ int semaphore_post(IDC_PTR, sem_t* s) {
  semaphore_t *sem = (semaphore_t*)s->id;
  __sync_fetch_and_add(&sem->count, 1);
  //syscall_notify_counter(&sem->count);
  return 0;
}

__IDCIMPL__ int semaphore_wait(IDC_PTR, sem_t* s) {
  // FIFO: each thread gets a sequence number, only the thread with
  // the smallest sequence number may exit.
  semaphore_t *sem = (semaphore_t*)s->id;
  unsigned seq_nr = __sync_fetch_and_add(&sem->entry_nr, 1);
  while (sem->exit_nr != seq_nr || sem->count == 0) {
    syscall_yield();
    //syscall_wait_counter(&sem->exit_nr, seq_nr, &sem->count);
  }
  __sync_fetch_and_sub(&sem->count, 1);
  __sync_fetch_and_add(&sem->exit_nr, 1);
  return 0;
}
