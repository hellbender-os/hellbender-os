#ifndef _SEMAPHORE_H
#define _SEMAPHORE_H

#include <fcntl.h>       /* For O_* constants */
#include <sys/types.h>   /* For mode constants */

#define SEM_FAILED 1

typedef struct sem {
  void* id;
} sem_t;

sem_t *sem_open(const char *name, int oflag, ...);
//sem_t *sem_open(const char name, int oflag, mode_t mode, unsigned int value); // when oflag has O_CREAT.
int sem_close(sem_t *sem);
//int sem_unlink(const char *name);
int sem_post(sem_t *sem);
int sem_wait(sem_t *sem);
//int sem_trywait(sem_t *sem);
//int sem_timedwait(sem_t *sem, const struct timespec *abs_timeout);

#endif
