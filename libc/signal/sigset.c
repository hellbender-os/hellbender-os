#include <signal.h>

// sigset_t: 0 means signal is included, 1 means signal is not included.
// so that signal mask can be used as in "pending_signals & signal_mask".

int sigemptyset(sigset_t *set) {
  *set = ~0;
  return 0;
}

int sigfillset(sigset_t *set) {
  *set = 0;
  return 0;
}

int sigaddset(sigset_t *set, int signo) {
  *set &= ~(1 << (signo-1));
  return 0;
}

int sigdelset(sigset_t *set, int signo) {
  *set |= (1 << (signo-1));
  return 0;
}

int sigismember(const sigset_t *set, int signo) {
  return (*set & (1 << (signo-1))) == 0;
}
