#include <kernel/pic.h>
#include <kernel/io.h>


#define PIC1		0x20		/* IO base address for master PIC */
#define PIC2		0xA0		/* IO base address for slave PIC */
#define PIC1_COMMAND	PIC1
#define PIC1_DATA	(PIC1+1)
#define PIC2_COMMAND	PIC2
#define PIC2_DATA	(PIC2+1)
#define PIC_EOI		0x20		/* End-of-interrupt command code */

void pic_stage_3_init() {
  outb(PIC1_DATA, 0);        
  outb(PIC2_DATA, 0);        
  
  // reprogram the PIC
  outb(0x20, 0x11);
  outb(0xA0, 0x11);
  outb(0x21, 0x20);
  outb(0xA1, 0x28);
  outb(0x21, 0x04);
  outb(0xA1, 0x02);
  outb(0x21, 0x01);
  outb(0xA1, 0x01);
  outb(0x21, 0x0);
  outb(0xA1, 0x0);

  outb(0x21, 0xff);
  outb(0xa1, 0xff);
}

void pic_eoi(unsigned line) {
  if (line > 8) {
    outb(PIC2_COMMAND, PIC_EOI);
  }
  outb(PIC1_COMMAND, PIC_EOI);
}

int pic_check_39() {
  outb(PIC1_COMMAND, 0x0B);
  unsigned char irr = inb(PIC1_COMMAND);
  return irr & 0x80;
}

void pic_disable(unsigned interrupt) {
  int irq_line = interrupt - 32;
  if (irq_line > 8) irq_line -= 72;

  uint16_t port;
  uint8_t value;
  
  if(irq_line < 8) {
    port = PIC1_DATA;
  } else {
    port = PIC2_DATA;
    irq_line -= 8;
  }
  value = inb(port) | (1 << irq_line);
  outb(port, value);        

}

void pic_enable(unsigned interrupt) {
  int irq_line = interrupt - 32;
  if (irq_line > 8) irq_line -= 72;

  uint16_t port;
  uint8_t value;
  
  if(irq_line < 8) {
    port = PIC1_DATA;
  } else {
    port = PIC2_DATA;
    irq_line -= 8;
  }
  value = inb(port) & ~(1 << irq_line);
  outb(port, value);        
}
