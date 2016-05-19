#include "pic.h"
#include "io.h"
#include "thread.h"
#include "isr.h"
#include "rtc.h"

#define PIC1		0x20		/* IO base address for master PIC */
#define PIC2		0xA0		/* IO base address for slave PIC */
#define PIC1_COMMAND	PIC1
#define PIC1_DATA	(PIC1+1)
#define PIC2_COMMAND	PIC2
#define PIC2_DATA	(PIC2+1)
#define PIC_EOI		0x20		/* End-of-interrupt command code */

void pic_init() {
  outb(PIC1_DATA, 0);        
  outb(PIC2_DATA, 0);        
  
  // reprogram the PIC interrupt range 
  outb(0x20, 0x11); // start init sequence
  outb(0xA0, 0x11);
  outb(0x21, 0x20); // set pic1 irq offset
  outb(0xA1, 0x28); // set pic2 irq offset
  outb(0x21, 0x04); // enable slave at pic1 irq2
  outb(0xA1, 0x02); // set pic2 as slave
  outb(0x21, 0x01); // set pic1 to 8086 mode
  outb(0xA1, 0x01); // set pic2 to 8086 mode
  outb(0x21, 0x00); // enable all pic1 interrupts
  outb(0xA1, 0x00); // enable all pic2 interrupts
}

INLINE uint16_t pic_get_isr() {
  outb(PIC1_COMMAND, 0x0B);
  outb(PIC2_COMMAND, 0x0B);
  return (inb(PIC2_COMMAND) << 8) | inb(PIC1_COMMAND);
}

INLINE void pic_eoi(unsigned irq) {
  if (irq >= 8) outb(PIC2_COMMAND, PIC_EOI);
  outb(PIC1_COMMAND, PIC_EOI);
}

INLINE int pic_is_spurious(unsigned irq) {
  uint16_t isr = pic_get_isr();
  if (irq == 7) return isr != 128;
  else if (irq == 15) {
    if (isr != 0x8004) {
      pic_eoi(2);
      return 0;
    } else return 1;
  } else return 0;
}

void pic_isr(struct thread_state* state, unsigned ring, unsigned irq) {
  (void)state;
  (void)ring;

  unsigned pirq = irq - 32;
  if (!pic_is_spurious(pirq)) {
    if (pirq == 8) rtc_update(); // try to keep RTC latency as low as possible.
    pic_eoi(pirq);
    isr_send_signal(irq);
  }
}
