#include <kernel/io.h>
#include <kernel/pit.h>

#include <coresrv/rtc.h>

__IDCIMPL__ void rtc_beep(IDC_PTR, unsigned freq, unsigned ticks) {
  if (freq == 0 || ticks == 0) {
    rtc.beep_ticks = 0;
    uint8_t tmp = inb(0x61) & 0xFC;
    outb(0x61, tmp);
 
  } else {
    rtc.beep_ticks = ticks;
    uint32_t timer;
    uint8_t tmp;
 
    //Set the PIT to the desired frequency
    timer = PIT_BASE_FREQUENCY / freq;
    outb(PIT_COMMAND, PIT_SEL_CHAN2 | PIT_ACC_LOHI | PIT_MODE_SQG);
    outb(PIT_CHAN2_DATA, (uint8_t) (timer) );
    outb(PIT_CHAN2_DATA, (uint8_t) (timer >> 8));
    
    //And play the sound using the PC speaker
    tmp = inb(0x61);
    if (tmp != (tmp | 3)) {
      outb(0x61, tmp | 3);
    }
  }
}
