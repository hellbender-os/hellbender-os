#include "kernel.h"

// this is used by some inline assembler to ensure memory ordering.
unsigned long __force_order;

__attribute__((__noreturn__))
void kernel_panic() {
  // TODO: Add proper kernel panic.
  asm("cli;1:hlt;jmp 1b");
  __builtin_unreachable();
}

void kernel_main(struct MultibootData* data) {
  (void)data;
  BREAK;
  kernel_panic();
}

