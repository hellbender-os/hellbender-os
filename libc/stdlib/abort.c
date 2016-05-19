#include <stdlib.h>
#include <signal.h>

__attribute__((__noreturn__))
void abort(void) {
  raise(SIGABRT);
  __builtin_unreachable();
}
