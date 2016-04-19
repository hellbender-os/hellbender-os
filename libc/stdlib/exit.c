#include <stdlib.h>
#include <pthread.h>
#include <hellbender/syscall.h>

pthread_mutex_t exit_mutex;
extern void _fini();

void (*_exit_func)(void) = NULL;

void exit(int status) {
  pthread_mutex_lock(&exit_mutex);
  if (_exit_func) (*_exit_func)();
  _fini();
  syscall_exit(status);
  __builtin_unreachable();
}

void _exit(int status) {
  pthread_mutex_lock(&exit_mutex);
  _fini();
  syscall_exit(status);
  __builtin_unreachable();
}

void _Exit(int status) {
  pthread_mutex_lock(&exit_mutex);
  _fini();
  syscall_exit(status);
  __builtin_unreachable();
}

