#include <stdio.h>
#include <hellbender.h>
#include <coresrv/timer.h>

volatile unsigned counter = 0;

void timer_isr() {
  ++counter;
  syscall_iret();
}

unsigned coresrv_get_ticks() {
  return counter;
}
