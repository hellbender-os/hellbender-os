#include <kernel/kstdlib.h>
#include <kernel/kstdio.h>

__attribute__((__noreturn__))
void kabort(void) {
  // TODO: Add proper kernel panic.
  kprintf("Kernel Panic: abort()\n");
  asm("cli;1:hlt;jmp 1b");
  __builtin_unreachable();
}
