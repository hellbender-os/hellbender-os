#include <coresrv/rtc.h>
#include <kernel/pit.h>

__IDCIMPL__ uint64_t rtc_time(IDC_PTR) {
  uint64_t ticks_now = rtc.ticks;
  uint64_t delta_ticks = ticks_now - rtc.current_ticks;
  uint64_t pulses = rtc.current_pulses + delta_ticks * PIT_PULSES_PER_TICK;
  uint64_t new_seconds = pulses / PIT_BASE_FREQUENCY;
  rtc.current_time += new_seconds;
  rtc.current_ticks = ticks_now;
  rtc.current_pulses = pulses - new_seconds * PIT_BASE_FREQUENCY;
  return rtc.current_time;
}
