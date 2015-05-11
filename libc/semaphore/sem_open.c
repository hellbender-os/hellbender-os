#include <semaphore.h>
#include <hellbender.h>
#include <stdarg.h>
#include <stdlib.h>

sem_t *sem_open(const char *name, int oflag, ...) {
  sem_t *s = malloc(sizeof(sem_t));
  if (oflag & O_CREAT) {
    va_list arguments;
    va_start(arguments, oflag);
    mode_t mode = va_arg(arguments, mode_t);
    unsigned value = va_arg(arguments, unsigned);
    va_end(arguments);
    (void)(mode); (void)(value);
    //syscall_sem_create(s, name, oflag, mode, value);
    return NULL;
    
  } else {
    if (syscall_sem_open(s, name, oflag) == 0) {
      return s;
    } else {
      free(s);
      return NULL;
    }
  }
}
