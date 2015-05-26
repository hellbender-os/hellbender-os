#include <string.h>
#include <semaphore.h>
#include <coresrv/semaphore.h>

__IDCIMPL__ int semaphore_open(IDC_PTR, sem_t *sem, const char* name, int oflag) {
  (void)(oflag);//TODO
  for (semaphore_t *s = semaphores; s; s = s->next) {
    if (strcmp(s->name, name) == 0) {
      sem->named_id = s;
      return 0;
    }
  }
  return 1;
}
