#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <kernel/kernel.h>

void isr_routine_0() {
  printf("isr_routine_0\n");
  abort();
}

void isr_routine_1() {
  printf("isr_routine_1\n");
  abort();
}

void isr_routine_2() {
  printf("isr_routine_2\n");
  abort();
}

void isr_routine_3() {
  printf("isr_routine_3\n");
  abort();
}

void isr_routine_4() {
  printf("isr_routine_4\n");
  abort();
}

void isr_routine_5() {
  printf("isr_routine_5\n");
  abort();
}

void isr_routine_6() {
  printf("isr_routine_6\n");
  abort();
}

void isr_routine_7() {
  printf("isr_routine_7\n");
  abort();
}

void isr_routine_8() {
  printf("isr_routine_8\n");
  abort();
}

void isr_routine_9() {
  printf("isr_routine_9\n");
  abort();
}

void isr_routine_10() {
  printf("isr_routine_10\n");
  abort();
}

void isr_routine_11() {
  printf("isr_routine_11\n");
  abort();
}

void isr_routine_12(uint32_t *stack) {
  printf("isr_routine_12: SSF\n");
  uint32_t error_code = stack[0];
  printf("error code = %x\n", (unsigned)error_code);
  uint32_t code_address = stack[1];
  uint32_t code_selector = stack[2];
  printf("at %x:%x\n", (unsigned)code_selector, (unsigned)code_address);
  abort();
}

void isr_routine_13(uint32_t *stack) {
  printf("isr_routine_13: GPF\n");
  uint32_t error_code = stack[0];
  printf("error code = %x\n", (unsigned)error_code);
  uint32_t code_address = stack[1];
  uint32_t code_selector = stack[2];
  printf("at %x:%x\n", (unsigned)code_selector, (unsigned)code_address);
  abort();
}

extern unsigned long __force_order;

void isr_routine_14(uint32_t *stack) {
  printf("isr_routine_13: PF\n");
  uint32_t error_code = stack[0];
  printf("error code = %x\n", (unsigned)error_code);
  uint32_t code_address = stack[1];
  uint32_t code_selector = stack[2];
  uint32_t data_address;
  asm volatile("mov %%cr2,%0" : "=r"(data_address), "=m"(__force_order));
  printf("at %x:%x\n", (unsigned)code_selector, (unsigned)code_address);
  printf("reading %x\n", (unsigned)data_address);
  BREAK;
  abort();
}

void isr_routine_15() {
  printf("isr_routine_15\n");
  abort();
}

void isr_routine_16() {
  printf("isr_routine_16\n");
  abort();
}

void isr_routine_17() {
  printf("isr_routine_17\n");
  abort();
}

void isr_routine_18() {
  printf("isr_routine_18\n");
  abort();
}

void isr_routine_19() {
  printf("isr_routine_19\n");
  abort();
}

void isr_routine_20() {
  printf("isr_routine_20\n");
  abort();
}

void isr_routine_21() {
  printf("isr_routine_21\n");
  abort();
}

void isr_routine_22() {
  printf("isr_routine_22\n");
  abort();
}

void isr_routine_23() {
  printf("isr_routine_23\n");
  abort();
}

void isr_routine_24() {
  printf("isr_routine_24\n");
  abort();
}

void isr_routine_25() {
  printf("isr_routine_25\n");
  abort();
}

void isr_routine_26() {
  printf("isr_routine_26\n");
  abort();
}

void isr_routine_27() {
  printf("isr_routine_27\n");
  abort();
}

void isr_routine_28() {
  printf("isr_routine_28\n");
  abort();
}

void isr_routine_29() {
  printf("isr_routine_29\n");
  abort();
}

void isr_routine_30() {
  printf("isr_routine_30\n");
  abort();
}

void isr_routine_31() {
  printf("isr_routine_31\n");
  abort();
}
