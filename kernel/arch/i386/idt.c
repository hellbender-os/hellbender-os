#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <kernel/kernel.h>
#include <kernel/kstdlib.h>
#include <kernel/kstdio.h>
#include <kernel/idt.h>

#define INT_COUNT 256
uint8_t idt_table[INT_COUNT*8];
uint32_t idt_routines[INT_COUNT];
uint32_t idt_wrappers[INT_COUNT];

extern void idt_wrapper_50();

typedef struct __attribute__((packed)) idt_address {
  uint16_t size;
  uint32_t address;
} idt_address_t;

void idt_dispatcher() {
  kprintf("test\n");
}

void idt_initialize() {
  memset(idt_table, 0, sizeof(idt_table));
  memset(idt_wrappers, 0, sizeof(idt_wrappers));
  idt_wrappers[0x50] = (uint32_t)&idt_wrapper_50;
}

void idt_set_entry(unsigned interrupt, idt_entry_t source) {
  if (idt_wrappers[interrupt] == 0) {
    kprintf("Specified interrupt is not supported!\n");
    kabort();
  }
  
  uint8_t* target  = idt_table + interrupt*8;
  
  // Encode the selector
  target[2] = SEL_KERNEL_CODE & 0xFF;
  target[3] = (SEL_KERNEL_CODE >> 8) & 0xFF;

  // Encode the offset (of the wrapper)
  idt_routines[interrupt] = (source.offset);
  uint32_t offset = idt_wrappers[interrupt];
  target[0] = offset & 0xFF;
  target[1] = (offset >> 8) & 0xFF;
  target[6] = (offset >> 16) & 0xFF;
  target[7] = (offset >> 24) & 0xFF;

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
       //       "sti"
       : /* no output registers */
       : "a"(&idt)
       : 
       );
}

void idt_send_interrupt(unsigned interrupt) {
  switch (interrupt) {
  case 0x50: asm ("int $0x50"); break;
  default:
    kprintf("Specified interrupt is not supported!\n");
    kabort();
  }
}
