#include "idt.h"
#include "config.h"
#include "lomem.h"
#include "page.h"
#include "kernel.h"

#include <stdint.h>

struct idt_entry {
  uint16_t offset_0;
  uint16_t selector;
  uint8_t ist;
  uint8_t attributes;
  uint16_t offset_1;
  uint32_t offset_2;
  uint32_t reserved;
}  __attribute__((packed));

#define IDT_VECTORS 256
struct idt_entry *idt_table;

struct __attribute__((packed)) {
  uint16_t size;
  uint64_t address;
} idt_ptr __attribute__((aligned(0x20))) = {
  (uint16_t)sizeof(struct idt_entry) * IDT_VECTORS, 
  (uint64_t)/*idt_table -- allocated runtime*/0
 };

#define idt_set(INTERRUPT) \
  extern void isr_wrapper_ ## INTERRUPT (); \
  idt_set_entry(INTERRUPT, (uint64_t)&isr_wrapper_ ## INTERRUPT)

static void idt_set_entry(unsigned interrupt, uint64_t offset) {
  struct idt_entry* target  = idt_table + interrupt;
  
  target->selector = CODE_SELECTOR;
  target->offset_0 = offset & 0xFFFF;
  target->offset_1 = (offset >> 16) & 0xFFFF;
  target->offset_2 = (offset >> 32) & 0xFFFFFFFF;

  // encode attributes:
  uint16_t ist = 0; // interrupt stack table (to use a clean stack for NMI and otherse).
  uint16_t type = 0xE; // 64-bit Interrupt Gate
  uint16_t dpl = 0; // descriptor privilege level
  uint16_t present = 1;
  target->ist = (ist & 7);
  target->attributes = ((present & 1)<<7) | ((dpl & 3)<<5) | (type & 15);
}

void idt_init() {
  // allocate IDT
  if (idt_ptr.size > 4096) kernel_panic();
  idt_table = (struct idt_entry*)kernel_p2v(page_clear(lomem_alloc_4k()));
  idt_ptr.address = (uintptr_t)idt_table;

  // traps
  idt_set(0);
  idt_set(1);
  idt_set(2);
  idt_set(3);
  idt_set(4);
  idt_set(5);
  idt_set(6);
  idt_set(7);
  idt_set(8);
  idt_set(9);
  idt_set(10);
  idt_set(11);
  idt_set(12);
  idt_set(13);
  idt_set(14);
  idt_set(15);
  idt_set(16);
  idt_set(17);
  idt_set(18);
  idt_set(19);
  idt_set(20);
  idt_set(30);

  // PIC interrupts
  idt_set(32);
  idt_set(33);
  idt_set(34);
  idt_set(35);
  idt_set(36);
  idt_set(37);
  idt_set(38);
  idt_set(39);
  idt_set(40);
  idt_set(41);
  idt_set(42);
  idt_set(43);
  idt_set(44);
  idt_set(45);
  idt_set(46);
  idt_set(47);

  asm volatile ("lidt (%0);"
       : /* no output registers */
       : "r"(&idt_ptr)
       : 
       );
}
