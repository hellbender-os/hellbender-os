#include "kernel.h"
#include "multiboot.h"

// this is used by some inline assembler to ensure memory ordering.
unsigned long __force_order;

__attribute__((__noreturn__))
void kernel_panic() {
  // TODO: Add proper kernel panic.
  asm("cli;1:hlt;jmp 1b");
  __builtin_unreachable();
}

void kernel_add_cpu(struct cpu* cpu) {
  (void)cpu;
}

void kernel_main() {
  BREAK;
  kernel_panic();
}

