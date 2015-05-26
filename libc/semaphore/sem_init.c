#include <semaphore.h>
#include <coresrv/semaphore.h>

int sem_init(sem_t *sem, int pshared, unsigned int value) {
  (void)(pshared); //TODO: all semaphores are shared by default.
  *sem = SEMAPHORE_INITIALIZER;
  sem->count = value;
  return 0;
}

