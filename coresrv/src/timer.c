#include <hellbender.h>

static unsigned counter = 0;

void timer_isr() {
  if (++counter % 18 == 0) {
    syscall_print("second.\n");
  }
  syscall_iret();
}
