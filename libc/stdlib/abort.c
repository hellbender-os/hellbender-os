#include <stdlib.h>
#include <stdio.h>

#if defined(__is_hellbender_kernel)

__attribute__((__noreturn__))
void abort(void) {
  // TODO: Add proper kernel panic.
  printf("Kernel Panic: abort()\n");
  asm("cli;1:hlt;jmp 1b");
  __builtin_unreachable();
  
}

#else
#include <hellbender.h>

__attribute__((__noreturn__))
void abort(void) {
  syscall_exit(-1);
}

#endif
