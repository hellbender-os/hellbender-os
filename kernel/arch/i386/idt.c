#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <kernel/kernel.h>
#include <kernel/kstdlib.h>
#include <kernel/kstdio.h>
#include <kernel/idt.h>

uint8_t idt_table[IDT_VECTORS*8];

typedef struct __attribute__((packed)) idt_address {
  uint16_t size;
  uint32_t address;
} idt_address_t;

void idt_initialize() {
  memset(idt_table, 0, sizeof(idt_table));
}

void idt_set_entry(unsigned interrupt, idt_entry_t source) {
  uint8_t* target  = idt_table + interrupt*8;
  
  // Encode the selector
  target[2] = SEL_KERNEL_CODE & 0xFF;
  target[3] = (SEL_KERNEL_CODE >> 8) & 0xFF;

  // Encode the offset (of the wrapper)
  target[0] = source.offset & 0xFF;
  target[1] = (source.offset >> 8) & 0xFF;
  target[6] = (source.offset >> 16) & 0xFF;
  target[7] = (source.offset >> 24) & 0xFF;

  // Encode attributes
  target[5] = 0x80; // present
  target[5] |= (source.dpl & 3) << 5;
  target[5] |= (source.type & 15);
}

void idt_enable_interrupts() {
#ifdef DEBUG
  kprintf("idt_enable_interrupts\n");
#endif

  volatile idt_address_t idt // needs volatile because some wierd optimization.
    = (idt_address_t) { .address = (uint32_t)idt_table,
                        .size = sizeof(idt_table)-1 };
  asm ("lidt (%%eax);"
       "sti"
       : /* no output registers */
       : "a"(&idt)
       : 
       );
}
