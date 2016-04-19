#include <stdlib.h>
#include <pthread.h>
#include <hellbender/syscall.h>

extern pthread_mutex_t _libc_mutex;
extern void _fini();

void (*_exit_func)(void) = NULL;

void exit(int status) {
  pthread_mutex_lock(&_libc_mutex);
  if (_exit_func) (*_exit_func)();
  _fini();
  syscall_exit(status);
  pthread_mutex_unlock(&_libc_mutex);
}

void _exit(int status) {
  pthread_mutex_lock(&_libc_mutex);
  _fini();
  syscall_exit(status);
  pthread_mutex_unlock(&_libc_mutex);
}

void _Exit(int status) {
  pthread_mutex_lock(&_libc_mutex);
  _fini();
  syscall_exit(status);
  pthread_mutex_unlock(&_libc_mutex);
}

