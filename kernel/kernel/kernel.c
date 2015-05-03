#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <kernel/kernel.h>

kernel_t kernel;

#define KERNEL_STACK_SIZE 16384
uint8_t kernel_stack[KERNEL_STACK_SIZE] __attribute__((aligned(PAGE_SIZE)));
const unsigned kernel_stack_size = KERNEL_STACK_SIZE;

kernel_t* kernel_ptr() {
  return &kernel;
}

void kernel_init_interrupts() {
  // IDT
}

void kernel_main(void) {
  printf("Hello, kernel World!\n");
}
