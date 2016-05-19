#include <unistd.h>

#include <hellbender/ctrl.h>

pid_t getpid(void) {
  return ctrl.get_pid();
}

pid_t getppid(void) {
  return ctrl.get_ppid();
}
