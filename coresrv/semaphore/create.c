#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>

#include <coresrv/semaphore.h>
#include <kernel/syscall.h>

semaphore_t *semaphores = NULL;

__IDCIMPL__ int semaphore_create(IDC_PTR, sem_t *sem, const char* name, int oflag, mode_t mode, unsigned value) {
  (void)(oflag);//TODO
  (void)(mode);//TODO
  size_t len = strlen(name) + 1;
  if (len > SEMAPHORE_MAX_NAME) {
    printf("Maximum semaphore length exceeded.\n");
    abort();
  }

  semaphore_t* s = (semaphore_t*)malloc(sizeof(semaphore_t) + len);
  memset(s, 0, sizeof(semaphore_t));
  sem_init(&s->sem, 1, value);
  if (len) {
    s->name = ((char*)s) + sizeof(semaphore_t);
    memcpy((char*)s->name, name, len);
    s->next = semaphores;
    semaphores = s;
  }
  sem->named_id = s;
  return 0;
}
