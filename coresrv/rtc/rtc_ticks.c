#include <stdint.h>
#include <coresrv/rtc.h>

extern volatile uint32_t counter;

__IDCIMPL__ unsigned coresrv_rtc_ticks(IDC_PTR) {
  return counter;
}
