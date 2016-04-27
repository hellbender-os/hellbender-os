#include <hellbender/debug.h>
#include <hellbender/syscall.h>
#include "vga.h"

#include <signal.h>

int test_srv(int i) {
  return i;
}
typedef int (*test_srv_t)(int);

int main(int argc, char* argv[]) {
  VGA_AT(0,9) = VGA_ENTRY('r', WHITE_ON_BLACK);
  (void)argc;
  (void)argv;

  // test service calls.
  test_srv_t srv = (test_srv_t)syscall_register_service(test_srv);
  BREAK;
  int test = srv(2);
  if (test == 2) {
    VGA_AT(0,9) = VGA_ENTRY('R', WHITE_ON_BLACK);
  }

  // test interrupt processing with signals.
  syscall_set_signal_mask(~0ull);
  syscall_set_irq_signal(33, SIGUSR1);
  for (int signum = 0; !signum; ) {
    signum = syscall_wait_signal(1<<SIGUSR1);
  }
}
