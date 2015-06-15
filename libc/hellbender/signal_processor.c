#include <hellbender/signal_wrapper.h>
#include <hellbender/threadlocal.h>

#include <signal.h>

void _signal_processor(int signum,
                       siginfo_t* info,
                       void* context,
                       struct sigaction* action) {
  // set signal mask while processing the signal.
  uint32_t old_mask = THREADLOCAL->signal_mask;
  THREADLOCAL->signal_mask &= (uint32_t)action->sa_mask;
  if ((action->sa_flags & SA_NODEFER) == 0) {
    THREADLOCAL->signal_mask &= ~(uint32_t)(1 << (signum - 1));
  }

  // SA_RESETHAND => reset action.
  struct sigaction copy = *action;
  if (action->sa_flags & SA_RESETHAND) {
    action->sa_handler = SIG_DFL;
    action->sa_flags &= ~SA_SIGINFO;
  }

  // call correct action.
  if (copy.sa_flags & SA_SIGINFO) {
    copy.sa_sigaction(signum, info, context);
  } else {
    copy.sa_handler(signum);
  }
  
  // restore signal mask.
  THREADLOCAL->signal_mask = old_mask;
}

