#include <semaphore.h>
#include <coresrv/semaphore.h>

int sem_init(sem_t *sem, int pshared, unsigned int value) {
  (void)(pshared); //TODO: all semaphores are shared by default.
  if (CORE_IDC(semaphore_create, sem, "", 0, 0, value) == 0) {
    return 0;
  } else {
    return -1;
  }
}

