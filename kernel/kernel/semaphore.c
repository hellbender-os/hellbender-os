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

semaphore_t* semaphore_create(const char* name, unsigned value) {
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
    s->next = kernel.semaphores;
    kernel.semaphores = s;
  }
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

wait_result_t semaphore_wait_still(wait_state_t* state) {
  semaphore_t* s = (semaphore_t*)state->wait_obj;
  if (s->exit_nr == state->seq_nr) {
    if (s->count) {
      __sync_fetch_and_sub(&s->count, 1);
      __sync_fetch_and_add(&s->exit_nr, 1);
      return WAIT_NO_MORE;
    }
  }
  return WAIT_STILL;
}

wait_result_t semaphore_wait(semaphore_t* s, wait_state_t* state) {
  // FIFO: each thread gets a sequence number, only the thread with
  // the smallest sequence number may exit.
  state->wait_func = &semaphore_wait_still;
  state->wait_obj = s;
  state->seq_nr = __sync_fetch_and_add(&s->entry_nr, 1);
  return semaphore_wait_still(state);
}
