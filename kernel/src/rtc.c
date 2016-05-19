#include "rtc.h"

#include "config.h"
#include "io.h"
#include "spin.h"

// global time info
struct rtc {
  spinlock_raw_t lock;
  struct rtc_data {
    uint64_t seconds_at_boot; // calculated at boot
    uint64_t counter_at_boot;
    uint64_t seconds_since_boot; // updated every interrupt
    uint64_t counter_since_boot;
  } data;
} rtc;

INLINE uint64_t read_counter() {
  register uint64_t tick1, tick2;
  asm volatile ("rdtsc" : "=a"(tick1), "=d"(tick2) :: "memory");
  return (tick2<<32)|tick1;
}

INLINE unsigned read_reg(unsigned reg) {
  outb(0x70, reg);
  return inb(0x71);
}

INLINE void wait_rtc_update() {
  read_reg(0xC); // clear old interrupt status flags.
  while ((read_reg(0xC) & 0x10) == 0);
}

INLINE unsigned bcd_to_bin(unsigned bcd) {
  return (bcd >> 4) * 10 + (bcd & 0xf);
}

// from http://howardhinnant.github.io/date_algorithms.html
INLINE int days_from_civil(int y, unsigned m, unsigned d) {
    y -= m <= 2;
    int era = (y >= 0 ? y : y-399) / 400;
    unsigned yoe = (unsigned)(y - era * 400);                // [0, 399]
    unsigned doy = (153*(m + (m > 2 ? -3 : 9)) + 2)/5 + d-1; // [0, 365]
    unsigned doe = yoe * 365 + yoe/4 - yoe/100 + doy;        // [0, 146096]
    return era * 146097 + (int)doe - 719468;
}

static uint64_t read_seconds() {
  // read and decode rtc time.
  unsigned seco = read_reg(0);
  unsigned minu = read_reg(2);
  unsigned hour = read_reg(4);
  unsigned daym = read_reg(7);
  unsigned mont = read_reg(8);
  unsigned year = read_reg(9);
  unsigned reg_b = read_reg(0x0B);
  if ((reg_b & 0x4) == 0) { // bcd mode
    seco = bcd_to_bin(seco);
    minu = bcd_to_bin(minu);
    hour = bcd_to_bin(hour & 0x7f) | (hour & 0x80); // keep am/pm bit
    daym = bcd_to_bin(daym);
    mont = bcd_to_bin(mont);
    year = bcd_to_bin(year);
  }
  if ((reg_b & 0x2) == 0) { // 12 hour format
    if (hour & 0x80) { // pm
      if (hour == (0x80+12)) hour = 12;
      else hour = (hour & 0x7f) + 12;
      hour = (hour & 0x7f);
    } else { // am
      if (hour == 12) hour = 0;
    }
  }
  if (year > 15) year += 2000;
  else year += 2100;

  // convert date-time to seconds since epoch.
  int dayz = days_from_civil(year, mont, daym);
  return (  (uint64_t)dayz * 86400
          + (uint64_t)hour * 3600
          + (uint64_t)minu * 60
          + (uint64_t)seco);
}

void rtc_init() {
  // read counter and time at boot:
  wait_rtc_update();
  rtc.data.counter_at_boot = read_counter();
  rtc.data.seconds_at_boot = read_seconds();

  // read counter and time one second later:
  wait_rtc_update();
  rtc.data.counter_since_boot = read_counter() - rtc.data.counter_at_boot;
  rtc.data.seconds_since_boot = read_seconds() - rtc.data.seconds_at_boot;

  // enable the interrupt.
  outb(0x70, 0x80|0xB); // also disables NMI.
  unsigned prev = inb(0x71);
  outb(0x70, 0x80|0xB);
  outb(0x71, prev|0x10); // enables update interrupt.
  outb(0x70, 0); // restore NMI.
}

void rtc_update() {
  uint64_t counter = read_counter();
  SPIN_GUARD_RAW(rtc.lock);
  rtc.data.counter_since_boot = counter - rtc.data.counter_at_boot;
  ++rtc.data.seconds_since_boot;
}

uint64_t rtc_seconds_at_boot() {
  SPIN_GUARD_RAW(rtc.lock);
  return rtc.data.seconds_at_boot;
}

uint64_t rtc_nsec_since_boot() {
  uint64_t counter = read_counter();
  struct rtc_data data;
  {
    SPIN_GUARD_RAW(rtc.lock);
    data = rtc.data;
  }
  __uint128_t nano_counts = counter - data.counter_at_boot;
  nano_counts *= 1000000000;
  nano_counts *= data.seconds_since_boot;
  nano_counts /= data.counter_since_boot;
  return (uint64_t)nano_counts;
}

