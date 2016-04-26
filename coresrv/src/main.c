#include <hellbender/debug.h>
#include <hellbender/syscall.h>
#include "vga.h"

#include <signal.h>

int main(int argc, char* argv[]) {
  VGA_AT(0,9) = VGA_ENTRY('r', WHITE_ON_BLACK);
  (void)argc;
  (void)argv;
  BREAK;
  syscall_set_signal_mask(~0ull);
  syscall_set_irq_signal(33, SIGUSR1);
  for (int signum = 0; !signum; ) {
    signum = syscall_wait_signal(1<<SIGUSR1);
  }
}
