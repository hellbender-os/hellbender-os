#include <stdlib.h>
#include <stdio.h>

#include <kernel/kernel.h>
#include <kernel/thread.h>
#include <kernel/isr.h>
#include <kernel/pic_isr.h>
#include <kernel/pic.h>

void isr_routine_81() {
  // interrupt request completed.
  //printf("isr_routine_81: %u\n", CURRENT_THREAD->pic_line);
  if (CURRENT_THREAD->pic_line) {
    pic_eoi(CURRENT_THREAD->pic_line - 1);
    CURRENT_THREAD->pic_line = 0;
    CURRENT_THREAD->state = THREAD_STATE_DEAD;
    pic_isr_process_next();
    
  } else {
    printf("Non-interrupt thread called IRET!\n");
    abort();
  }
}
