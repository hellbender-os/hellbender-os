#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <kernel/kernel.h>
#include <kernel/kstdlib.h>
#include <kernel/kstdio.h>
#include <kernel/isr.h>
#include <kernel/idt.h>
#include <kernel/io.h>

#define isr_set_idt(INTERRUPT) \
  extern void isr_wrapper_ ## INTERRUPT (); \
  entry.offset = (uint32_t)&isr_wrapper_ ## INTERRUPT;  \
  idt_set_entry(INTERRUPT, entry);

void isr_remap_pic() {
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

void isr_initialize() {
  isr_remap_pic();
  
  idt_entry_t entry = (idt_entry_t) {
    .offset = 0,
    .type = IDT_TYPE_INTERRUPT_32,
    .dpl = 0
  };

  // set protected mode traps
  isr_set_idt(0);
  isr_set_idt(1);
  isr_set_idt(2);
  isr_set_idt(3);
  isr_set_idt(4);
  isr_set_idt(5);
  isr_set_idt(6);
  isr_set_idt(7);
  isr_set_idt(8);
  isr_set_idt(9);
  isr_set_idt(10);
  isr_set_idt(11);
  isr_set_idt(12);
  isr_set_idt(13);
  isr_set_idt(14);
  isr_set_idt(15);
  isr_set_idt(16);
  isr_set_idt(17);
  isr_set_idt(18);
  isr_set_idt(19);
  isr_set_idt(20);
  isr_set_idt(21);
  isr_set_idt(22);
  isr_set_idt(23);
  isr_set_idt(24);
  isr_set_idt(25);
  isr_set_idt(26);
  isr_set_idt(27);
  isr_set_idt(28);
  isr_set_idt(29);
  isr_set_idt(30);
  isr_set_idt(31);

  // set PIC interrupts
  isr_set_idt(32);
  isr_set_idt(33);
  isr_set_idt(34);
  isr_set_idt(35);
  isr_set_idt(36);
  isr_set_idt(37);
  isr_set_idt(38);
  isr_set_idt(39);
  isr_set_idt(112);
  isr_set_idt(113);
  isr_set_idt(114);
  isr_set_idt(115);
  isr_set_idt(116);
  isr_set_idt(117);
  isr_set_idt(118);
  isr_set_idt(119);

  // set kernel interrupts
  isr_set_idt(80);
  isr_set_idt(81);
  isr_set_idt(82);
  isr_set_idt(83);
  isr_set_idt(84);
  isr_set_idt(85);
  isr_set_idt(86);
  isr_set_idt(87);
}

#define PIC1		0x20		/* IO base address for master PIC */
#define PIC2		0xA0		/* IO base address for slave PIC */
#define PIC1_COMMAND	PIC1
#define PIC1_DATA	(PIC1+1)
#define PIC2_COMMAND	PIC2
#define PIC2_DATA	(PIC2+1)
#define PIC_EOI		0x20		/* End-of-interrupt command code */
 
void isr_pic_eoi(unsigned interrupt) {
  if (interrupt > 40) {
    outb(PIC2_COMMAND, PIC_EOI);
  }
  outb(PIC1_COMMAND, PIC_EOI);
}

int isr_pic_check_39() {
  outb(PIC1_COMMAND, 0x0B);
  unsigned char irr = inb(PIC1_COMMAND);
  return irr & 0x80;
}

void isr_pic_disable(unsigned interrupt) {
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

void isr_pic_enable(unsigned interrupt) {
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
