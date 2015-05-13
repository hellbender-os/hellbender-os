#include <stdint.h>
#include <coresrv/rtc.h>

extern volatile uint32_t counter;

unsigned _idc_coresrv_rtc_ticks(__attribute__((unused))uintptr_t retval) {
  return counter;
}
