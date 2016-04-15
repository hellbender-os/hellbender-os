#include "kernel.h"
#include "multiboot.h"

// this is used by some inline assembler to ensure memory ordering.
unsigned long __force_order;

struct kernel kernel;

__attribute__((__noreturn__))
void kernel_panic() {
  // TODO: Add proper kernel panic.
  asm("cli;1:hlt;jmp 1b");
  __builtin_unreachable();
}

void kernel_add_cpu(struct cpu* cpu) {
  SPIN_GUARD_RAW(kernel.lock);
  if (kernel.n_cpus == KERNEL_MAX_CPUS) kernel_panic();
  kernel.cpus[kernel.n_cpus++] = cpu;
}

void kernel_main() {
  BREAK;
  kernel_panic();
}

