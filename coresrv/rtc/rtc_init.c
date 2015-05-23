#include <stdio.h>
#include <string.h>

#include <kernel/kernel.h>
#include <kernel/io.h>
#include <coresrv/rtc.h>

struct rtc rtc;

static unsigned read_reg(unsigned reg) {
  outb(0x70, reg);
  iowait();
  return inb(0x71);
}

static unsigned bcd_to_dec(unsigned bcd) {
  return (bcd >> 4) * 10 + (bcd & 0xf);
}

void rtc_init() {
  memset(&rtc, 0, sizeof(rtc));
  
  unsigned seco_bcd = read_reg(0);
  unsigned minu_bcd = read_reg(2);
  unsigned hour_bcd = read_reg(4);
  //unsigned dayw_bcd = read_reg(6);
  unsigned daym_bcd = read_reg(7);
  unsigned mont_bcd = read_reg(8);
  unsigned year_bcd = read_reg(9);

  rtc.current_ticks = rtc.ticks;
  BARRIER;

  unsigned year   = 2000 + bcd_to_dec(year_bcd);
  unsigned month  = bcd_to_dec(mont_bcd);
  unsigned day    = bcd_to_dec(daym_bcd);
  unsigned hour   = bcd_to_dec(hour_bcd);
  unsigned minute = bcd_to_dec(minu_bcd);
  unsigned second = bcd_to_dec(seco_bcd);

  // this is valid only until 2400..
  unsigned f_year_month = month <= 2 ? year - 1 : year;
  unsigned g_month = month <= 2 ? month + 13 : month + 1;
  unsigned days_now = 1461 * f_year_month / 4 + 153 * g_month / 5 + day;
  unsigned days_epoch = 1461 * (1970-1) / 4 + 153 * (1+13) / 5 + 1;
  unsigned days_since_epoch = days_now - days_epoch;

  rtc.current_time = ((uint64_t)days_since_epoch * 86400
                      + (uint64_t)hour * 3600
                      + (uint64_t)minute * 60
                      + (uint64_t)second);

  printf("RTC date: %u.%u.%u; ", day, month, year);
  printf("RTC time: %u:%u:%u; ", hour, minute, second);
  printf("Unix time: %u\n", (unsigned)rtc.current_time);
}
