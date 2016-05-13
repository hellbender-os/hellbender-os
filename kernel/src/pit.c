#include "pit.h"
#include <hellbender/io.h>

void pit_init() {
  outb(PIT_COMMAND, PIT_SEL_CHAN0 | PIT_ACC_LOHI | PIT_MODE_ITC);

  /*
  uint32_t timer = PIT_BASE_FREQUENCY / PIT_TICKS_PER_SECOND;
  
  outb(PIT_COMMAND, PIT_SEL_CHAN0 | PIT_ACC_LOHI | PIT_MODE_RGN);
  outb(PIT_CHAN0_DATA, (uint8_t) (timer) );
  outb(PIT_CHAN0_DATA, (uint8_t) (timer >> 8));
  */
}
