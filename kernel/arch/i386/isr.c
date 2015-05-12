#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <kernel/kernel.h>
#include <kernel/isr.h>
#include <kernel/idt.h>

#define isr_set_idt(INTERRUPT) \
  extern void isr_wrapper_ ## INTERRUPT (); \
  if (entry.offset) idt_set_entry(INTERRUPT, entry); \
  else { \
    entry.offset = (uint32_t)&isr_wrapper_ ## INTERRUPT; \
    idt_set_entry(INTERRUPT, entry); \
    entry.offset = 0; \
  }

static void isr_set_traps(idt_entry_t entry);
static void isr_set_pics(idt_entry_t entry);
static void isr_set_syscalls(idt_entry_t entry);

extern void isr_debugger();

void isr_stage_1_debugger() {
  idt_entry_t entry = (idt_entry_t) {
    .offset = (uint32_t)&isr_debugger,
    .type = IDT_TYPE_INTERRUPT_32,
    .dpl = 0
  };
  isr_set_traps(entry);
  isr_set_pics(entry);
  isr_set_syscalls(entry);
}

void isr_stage_3_trap() {
  idt_entry_t entry = (idt_entry_t) {
    .offset = 0,
    .type = IDT_TYPE_INTERRUPT_32,
    .dpl = 0
  };
  isr_set_traps(entry);
}

void isr_stage_3_syscall() {
  idt_entry_t entry = (idt_entry_t) {
    .offset = 0,
    .type = IDT_TYPE_INTERRUPT_32,
    .dpl = 0
  };
  isr_set_syscalls(entry);
}

void isr_stage_4_pic() {
  idt_entry_t entry = (idt_entry_t) {
    .offset = 0,
    .type = IDT_TYPE_INTERRUPT_32,
    .dpl = 0
  };
  isr_set_pics(entry);
}

static void isr_set_traps(idt_entry_t entry) {
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
}

static void isr_set_pics(idt_entry_t entry) {
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
}

static void isr_set_syscalls(idt_entry_t entry) {
  // set kernel interrupts
  entry.dpl = 3;
  isr_set_idt(80);
  isr_set_idt(81);
  isr_set_idt(82);
  isr_set_idt(83);
}
