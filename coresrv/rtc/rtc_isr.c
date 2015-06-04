#include <stdio.h>
#include <stdint.h>
#include <hellbender/hellbender.h>
#include <coresrv/rtc.h>

void rtc_isr() {
  ++rtc.ticks;
  if (rtc.beep_ticks) {
    if (!(--rtc.beep_ticks)) {
      rtc_beep(NO_IDC, 0, 0);
    }
  }
  syscall_iret();
}
