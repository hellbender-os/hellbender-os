#include <hellbender.h>

volatile unsigned counter = 0;

void timer_isr() {
  ++counter;
  syscall_iret();
}
