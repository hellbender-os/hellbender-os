#include <stdio.h>
#include <stdint.h>
#include <hellbender.h>
#include <coresrv/rtc.h>

volatile uint32_t counter = 0;
extern volatile uint32_t beep_counter;

void rtc_isr() {
  ++counter;
  if (beep_counter) {
    if (!(--beep_counter)) {
      coresrv_rtc_beep(NO_IDC, 0, 0);
    }
  }
  syscall_iret();
}
