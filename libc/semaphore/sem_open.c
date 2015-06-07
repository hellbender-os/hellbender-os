#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <semaphore.h>
#include <coresrv/semaphore.h>

#include <../fcntl/fcntl_impl.h>

sem_t *sem_open(const char *name, int oflag, ...) {
  if (strlen(name) >= SEMAPHORE_MAX_NAME) {
    return SEM_FAILED;
  }
  
  sem_t *s = malloc(sizeof(sem_t));
  *s = SEMAPHORE_INITIALIZER;
  if (oflag & O_CREAT) {
    va_list arguments;
    va_start(arguments, oflag);
    mode_t mode = va_arg(arguments, mode_t);
    mode &= _fcntl_data.umask;
    unsigned value = va_arg(arguments, unsigned);
    va_end(arguments);
    if (CORE_IDC(semaphore_create, s, name, oflag, mode, value) == 0) {
      return s;
    } else {
      free(s);
      return SEM_FAILED;
    }
    
  } else {
    if (CORE_IDC(semaphore_open, s, name, oflag) == 0) {
      return s;
    } else {
      free(s);
      return SEM_FAILED;
    }
  }
}
