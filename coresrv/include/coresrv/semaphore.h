#ifndef _CORESRV_SEMAPHORE
#define _CORESRV_SEMAPHORE

#include <coresrv/coresrv.h>
#include <sys/types.h>
#include <semaphore.h>

// maximum length of semaphore name buffer (including trailing 0).
#define SEMAPHORE_MAX_NAME 32

typedef struct sem sem_t;

typedef struct semaphore {
  struct semaphore* next;
  const char* name;
  sem_t sem;
} semaphore_t;

extern semaphore_t *semaphores;

__IDC__ int semaphore_create(IDC_PTR, sem_t *sem, const char* name, int oflag, mode_t mode, unsigned value);
__IDC__ int semaphore_open(IDC_PTR, sem_t *sem, const char* name, int oflag);
__IDC__ int semaphore_post(IDC_PTR, sem_t* sem);
__IDC__ int semaphore_wait(IDC_PTR, sem_t* sem);

#endif
