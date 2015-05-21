#include <stdio.h>
#include <kernel/io.h>

static unsigned read_reg(unsigned reg) {
  outb(0x70, reg);
  iowait();
  return inb(0x71);
}

void rtc_init() {
  unsigned seco_bdc = read_reg(0);
  unsigned minu_bdc = read_reg(2);
  unsigned hour_bdc = read_reg(4);
  //unsigned dayw_bdc = read_reg(6);
  unsigned daym_bdc = read_reg(7);
  unsigned mont_bdc = read_reg(8);
  unsigned year_bdc = read_reg(9);

  printf("RTC date: %x.%x.20%x; ", daym_bdc, mont_bdc, year_bdc);
  printf("RTC time: %x:%x:%x\n", hour_bdc, minu_bdc, seco_bdc);
}
