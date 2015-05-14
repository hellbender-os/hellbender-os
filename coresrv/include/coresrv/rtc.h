#ifndef _CORESRV_TIMER_H
#define _CORESRV_TIMER_H

#include <stdint.h>

extern unsigned coresrv_rtc_ticks();
extern void coresrv_rtc_beep(unsigned freq, unsigned ticks);

#endif
