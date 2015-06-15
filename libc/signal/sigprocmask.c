#include <signal.h>
#include <errno.h>

#include <hellbender/threadlocal.h>

int pthread_sigmask(int how, const sigset_t *set, sigset_t *oset) {
  if (oset) {
    *oset = (sigset_t)THREADLOCAL->signal_mask;
  }
  if (set) {
    switch (how) {
    case SIG_BLOCK:
      THREADLOCAL->signal_mask &= (uint32_t)*set;
      break;
    case SIG_SETMASK:
      THREADLOCAL->signal_mask = (uint32_t)*set;
      break;
    case SIG_UNBLOCK:
      THREADLOCAL->signal_mask |= ~(uint32_t)*set;
      break;
    default:
      errno = EINVAL;
      return -1;
    }
  }
  return 0;
}

int sigprocmask(int how, const sigset_t *set, sigset_t *oset) {
  // TODO: add process wide masks.
  
  pthread_sigmask(how, set, oset);
  return 0;
}
