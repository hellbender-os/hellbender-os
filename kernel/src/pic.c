#include "pic.h"
#include "io.h"

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
  outb(0x21, 0xFF); // disable all pic1 interrupts
  outb(0xA1, 0xFF); // disable all pic2 interrupts
}

