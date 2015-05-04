#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <kernel/kernel.h>
#include <kernel/kstdio.h>
#include <kernel/kstdlib.h>

void isr_routine_0() {
  kprintf("isr_routine_0\n");
  kabort();
}

void isr_routine_1() {
  kprintf("isr_routine_1\n");
  kabort();
}

void isr_routine_2() {
  kprintf("isr_routine_2\n");
  kabort();
}

void isr_routine_3() {
  kprintf("isr_routine_3\n");
  kabort();
}

void isr_routine_4() {
  kprintf("isr_routine_4\n");
  kabort();
}

void isr_routine_5() {
  kprintf("isr_routine_5\n");
  kabort();
}

void isr_routine_6() {
  kprintf("isr_routine_6\n");
  kabort();
}

void isr_routine_7() {
  kprintf("isr_routine_7\n");
  kabort();
}

void isr_routine_8() {
  kprintf("isr_routine_8\n");
  kabort();
}

void isr_routine_9() {
  kprintf("isr_routine_9\n");
  kabort();
}

void isr_routine_10() {
  kprintf("isr_routine_10\n");
  kabort();
}

void isr_routine_11() {
  kprintf("isr_routine_11\n");
  kabort();
}

void isr_routine_12() {
  kprintf("isr_routine_12\n");
  kabort();
}

void isr_routine_13(uint8_t *stack) {
  kprintf("isr_routine_13: GPF\n");
  uint32_t error_code = *((uint32_t*)(stack));
  kprintf("error code = %x\n", (unsigned)error_code);
  uint32_t code_address = *((uint32_t*)(stack+4));
  uint16_t code_selector = *((uint16_t*)(stack+8));
  kprintf("at %x:%x\n", (unsigned)code_selector, (unsigned)code_address);
  kabort();
}

extern unsigned long __force_order;

void isr_routine_14(uint8_t *stack) {
  kprintf("isr_routine_14: page fault\n");
  uint32_t error_code = *((uint32_t*)(stack));
  kprintf("error code = %x\n", (unsigned)error_code);
  uint32_t code_address = *((uint32_t*)(stack+4));
  uint16_t code_selector = *((uint16_t*)(stack+8));
  uint32_t data_address;
  asm volatile("mov %%cr2,%0" : "=r"(data_address), "=m"(__force_order));
  kprintf("at %x:%x\n", (unsigned)code_selector, (unsigned)code_address);
  kprintf("reading %x\n", (unsigned)data_address);
  kabort();
}

void isr_routine_15() {
  kprintf("isr_routine_15\n");
  kabort();
}

void isr_routine_16() {
  kprintf("isr_routine_16\n");
  kabort();
}

void isr_routine_17() {
  kprintf("isr_routine_17\n");
  kabort();
}

void isr_routine_18() {
  kprintf("isr_routine_18\n");
  kabort();
}

void isr_routine_19() {
  kprintf("isr_routine_19\n");
  kabort();
}

void isr_routine_20() {
  kprintf("isr_routine_20\n");
  kabort();
}

void isr_routine_21() {
  kprintf("isr_routine_21\n");
  kabort();
}

void isr_routine_22() {
  kprintf("isr_routine_22\n");
  kabort();
}

void isr_routine_23() {
  kprintf("isr_routine_23\n");
  kabort();
}

void isr_routine_24() {
  kprintf("isr_routine_24\n");
  kabort();
}

void isr_routine_25() {
  kprintf("isr_routine_25\n");
  kabort();
}

void isr_routine_26() {
  kprintf("isr_routine_26\n");
  kabort();
}

void isr_routine_27() {
  kprintf("isr_routine_27\n");
  kabort();
}

void isr_routine_28() {
  kprintf("isr_routine_28\n");
  kabort();
}

void isr_routine_29() {
  kprintf("isr_routine_29\n");
  kabort();
}

void isr_routine_30() {
  kprintf("isr_routine_30\n");
  kabort();
}

void isr_routine_31() {
  kprintf("isr_routine_31\n");
  kabort();
}
