#include <stdlib.h>
#include <hellbender.h>

extern void _fini();

void exit(int __attribute__((unused)) status) {
  _fini();
  syscall();
  __builtin_unreachable();
}
