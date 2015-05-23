#include <time.h>

#include <coresrv/rtc.h>

time_t time(time_t *time) {
  time_t retval = (time_t)CORE_IDC(rtc_time);
  if (time) *time = retval;
  return retval;
}


