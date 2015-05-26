#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>

#include <coresrv/semaphore.h>
#include <kernel/syscall.h>

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
