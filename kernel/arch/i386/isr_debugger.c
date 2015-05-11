#include <stdio.h>
#include <stdlib.h>

void isr_debugger(void* isr_stack, thread_state_t *params) {
  printf("Early interrupt!\n");
  abort();
}
