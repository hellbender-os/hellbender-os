#include <kernel/io.h>
#include <coresrv/rtc.h>

volatile uint32_t beep_counter = 0;

__IDCIMPL__ void coresrv_rtc_beep(IDC_PTR, unsigned freq, unsigned ticks) {
  if (freq == 0 || ticks == 0) {
    beep_counter = 0;
    uint8_t tmp = inb(0x61) & 0xFC;
    outb(0x61, tmp);
 
  } else {
    beep_counter = ticks;
    uint32_t timer;
    uint8_t tmp;
 
    //Set the PIT to the desired frequency
    timer = 1193180 / freq;
    outb(0x43, 0xb6);
    outb(0x42, (uint8_t) (timer) );
    outb(0x42, (uint8_t) (timer >> 8));
    
    //And play the sound using the PC speaker
    tmp = inb(0x61);
    if (tmp != (tmp | 3)) {
      outb(0x61, tmp | 3);
    }
  }
}
