#ifndef __HELLBENDER_KERNEL_RTC_H__
#define __HELLBENDER_KERNEL_RTC_H__

#include <stdint.h>

// Enables RTC update timer (1Hz), calculates seconds since Epoch.
void rtc_init();

// RTC update interrupt: updates the *_since_boot -fields.
void rtc_update(); // called directly by pic_isr (in pic.c).

uint64_t rtc_seconds_at_boot(); // seconds since epoch at boot time.
uint64_t rtc_nsec_since_boot(); // time since boot in nanoseconds.

#endif
