#include <kernel/kstdlib.h>
#include <kernel/kstdio.h>

__attribute__((__noreturn__))
void khalt(void) {
  kprintf("Kernel halted\n");
  while (1) { asm("hlt"); }
  __builtin_unreachable();
}
