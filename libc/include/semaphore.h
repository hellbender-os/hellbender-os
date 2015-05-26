#ifndef _SEMAPHORE_H
#define _SEMAPHORE_H

// The Open Group Base Specifications Issue 7

#include <fcntl.h>
#include <time.h>

typedef struct sem {
  void* named_id;
  volatile unsigned count;
  volatile unsigned entry_nr;
  volatile unsigned exit_nr;
} sem_t;

#define SEMAPHORE_INITIALIZER (sem_t){.named_id = (void*)0, .count = 0,. entry_nr = 0, .exit_nr = 0 }
#define SEM_FAILED ((sem_t*)0)

int    sem_close(sem_t *);
int    sem_destroy(sem_t *);
int    sem_getvalue(sem_t *restrict, int *restrict);
int    sem_init(sem_t *, int, unsigned);
sem_t *sem_open(const char *, int, ...);
int    sem_post(sem_t *);
int    sem_timedwait(sem_t *restrict, const struct timespec *restrict);
int    sem_trywait(sem_t *);
int    sem_unlink(const char *);
int    sem_wait(sem_t *);

#endif
