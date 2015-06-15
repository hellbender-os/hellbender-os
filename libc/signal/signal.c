#include <signal.h>

#include <hellbender/threadlocal.h>

void (*signal(int signum, void (*sigfunc)(int)))(int) {
  // TODO: check if signal cannot be caught or ignored.
  struct sigaction* action = &THREADLOCAL->signal_actions[signum-1];
  void (*retval)(int) = action->sa_handler;
  action->sa_handler = sigfunc;
  action->sa_mask = 0;
  action->sa_flags = 0;
  return retval;
}

