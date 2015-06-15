#include <signal.h>

#include <hellbender/threadlocal.h>

int sigaction(int sig, const struct sigaction *act, struct sigaction *oact) {
  // TODO: check if signal cannot be caught or ignored.
  struct sigaction* action = &THREADLOCAL->signal_actions[sig-1];
  if (oact) {
    *oact = *action;
  }
  if (act) {
    *action = *act;
  }
  return 0;
}
