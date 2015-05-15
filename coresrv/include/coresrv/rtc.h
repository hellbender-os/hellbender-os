#ifndef _CORESRV_TIMER_H
#define _CORESRV_TIMER_H

#include <coresrv/coresrv.h>

__IDC__ unsigned coresrv_rtc_ticks(IDC_PTR);
__IDC__ void coresrv_rtc_beep(IDC_PTR, unsigned freq, unsigned ticks);

#endif
