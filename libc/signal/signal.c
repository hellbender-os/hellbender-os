#include <signal.h>
#include <stdlib.h>

void (*signal(int signum, void (*sigfunc)(int)))(int) {
  (void)(signum);
  (void)(sigfunc);
  // TODO: support signals
  return NULL;
}

