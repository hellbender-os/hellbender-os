#include <hellbender/debug.h>
#include <hellbender/syscall.h>
#include <hellbender/libc_init.h>
#include <hellbender/coresrv_init.h>
#include "vga.h"

#include <signal.h>

int test_srv(int i) {
  return i;
}
typedef int (*test_srv_t)(int);

void broker_init();

int main(int argc, char* argv[]) {
  (void)argc;
  (void)argv;
  uint8_t foo[8192];
  if (foo[8191] == 0) {
    foo[8191] = 1;
  }

  struct coresrv_init *data = (struct coresrv_init *)_libc_init_get()->data;
  VGA_MEMORY((uint16_t*)data->vga_base);
  VGA_AT(0,9) = VGA_ENTRY('r', WHITE_ON_BLACK);

  broker_init();
  VGA_AT(0,8) = VGA_ENTRY('E', WHITE_ON_BLACK);

  // test interrupt processing with signals.
  syscall_set_signal_mask(~0ull);
  syscall_set_irq_signal(33, SIGUSR1);
  for (int signum = 0; !signum; ) {
    signum = syscall_wait_signal(1<<SIGUSR1);
  }
  VGA_AT(0,9) = VGA_ENTRY('R', WHITE_ON_BLACK);
}
