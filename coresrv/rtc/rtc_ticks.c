#include <stdint.h>
#include <coresrv/rtc.h>

__IDCIMPL__ uint64_t rtc_ticks(IDC_PTR) {
  return rtc.ticks;
}
