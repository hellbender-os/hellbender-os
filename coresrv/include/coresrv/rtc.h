#ifndef _CORESRV_TIMER_H
#define _CORESRV_TIMER_H

#include <stdint.h>
#include <coresrv/coresrv.h>

struct rtc {
  volatile uint64_t ticks;
  volatile uint32_t beep_ticks;

  uint64_t current_time; // seconds since Unix epoch; updated by rtc_time.
  uint64_t current_ticks;// value of ticks when current_time was updated.
  uint64_t current_pulses; // left-over pulses from last update.
};

extern struct rtc rtc;

void rtc_init();

__IDC__ uint64_t rtc_ticks(IDC_PTR);
__IDC__ uint64_t rtc_time(IDC_PTR);
__IDC__ void rtc_beep(IDC_PTR, unsigned freq, unsigned ticks);

#endif
