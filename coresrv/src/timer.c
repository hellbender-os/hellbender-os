#include <hellbender.h>

void timer_isr() {
  syscall_iret();
}
