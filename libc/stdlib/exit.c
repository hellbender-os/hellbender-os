#include <stdlib.h>
#include <hellbender.h>

extern void _fini();

void exit(int status) {
  _fini();
  syscall_exit(status);
}
