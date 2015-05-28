#include <sys/wait.h>

pid_t waitpid(pid_t pid, int *status, int options) {
  (void)(options);//TODO actual wait syscall.
  if (status) *status = _SYS_WAIT_EXITED;
  return pid;
}
