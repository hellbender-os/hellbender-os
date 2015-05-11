#include <stdlib.h>
#include <stdio.h>

#include <kernel/kernel.h>
#include <kernel/thread.h>

void isr_debugger(void* isr_stack, thread_state_t *params) {
  (void)(isr_stack);
  (void)(params);
  printf("early interrupt\n");
  abort();
}
