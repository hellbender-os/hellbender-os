#include <semaphore.h>
#include <hellbender.h>

int sem_init(sem_t *sem, int pshared, unsigned int value) {
  (void)(pshared); //TODO: all semaphores are shared by default.
  if (syscall_sem_create(sem, "", O_CREAT, 0, value) == 0) {
    return 0;
  } else {
    return -1;
  }
}

