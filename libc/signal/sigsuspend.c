#include <signal.h>
#include <unistd.h>
#include <errno.h>

#include <hellbender/threadlocal.h>

int sigsuspend(const sigset_t *sigmask) {
  uint32_t old_mask = THREADLOCAL->signal_mask;
  THREADLOCAL->signal_mask = (uint32_t)sigmask;
  sleep((unsigned)-1);
  THREADLOCAL->signal_mask = old_mask;
  errno = EINTR;
  return -1;
}

