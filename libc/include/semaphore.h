#ifndef _SEMAPHORE_H
#define _SEMAPHORE_H

// The Open Group Base Specifications Issue 7

#include <fcntl.h>
#include <time.h>

typedef struct sem {
  void* id;
} sem_t;

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
