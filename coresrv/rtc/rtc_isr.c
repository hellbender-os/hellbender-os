#include <stdio.h>
#include <stdint.h>
#include <hellbender.h>

volatile uint32_t counter = 0;

void rtc_isr() {
  ++counter;
  syscall_iret();
}
