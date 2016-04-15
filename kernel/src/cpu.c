#include "kernel.h"

// small stack to run the kernel.
uint8_t cpu_stack[CPU_STACK_SIZE] __attribute__ ((aligned (PAGE_SIZE)));
