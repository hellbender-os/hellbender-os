#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sched.h>

#include <kernel/semaphore.h>
#include <kernel/kernel.h>
#include <kernel/scheduler.h>

typedef struct semaphore {
  struct semaphore* next;
  const char* name;
  volatile unsigned count;
  volatile unsigned entry_nr;
  volatile unsigned exit_nr;
} semaphore_t;

semaphore_t* semaphore_create(const char* name) {
  size_t len = strlen(name) + 1;
  if (len > SEMAPHORE_MAX_NAME) {
    printf("Maximum semaphore length exceeded.\n");
    abort();
  }

  semaphore_t* s = (semaphore_t*)malloc(sizeof(semaphore_t) + len);
  memset(s, 0, sizeof(semaphore_t));
  s->name = ((char*)s) + sizeof(semaphore_t);
  memcpy((char*)s->name, name, len);
  s->next = kernel.semaphores;
  kernel.semaphores = s;
  return s;
}

semaphore_t* semaphore_find(const char* name) {
  for (semaphore_t *s = kernel.semaphores; s; s = s->next) {
    if (strcmp(s->name, name) == 0) {
      return s;
    }
  }
  return NULL;
}

void semaphore_post(semaphore_t* s) {
  __sync_fetch_and_add(&s->count, 1);
}

void semaphore_wait(semaphore_t* s) {
  // FIFO: each thread gets a sequence number, only the thread with
  // the smallest sequence number may exit.
  unsigned nr = __sync_fetch_and_add(&s->entry_nr, 1);
  while (1) {
    if (s->exit_nr == nr) {
      if (s->count) {
        __sync_fetch_and_sub(&s->count, 1);
        __sync_fetch_and_add(&s->exit_nr, 1);
        return;
      }
    }
    sched_yield();
  }
}
