#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <kernel/kernel.h>
#include <kernel/syscall.h>
#include <kernel/kstdio.h>
#include <kernel/kstdlib.h>
#include <kernel/isr.h>
#include <kernel/pic_isr.h>
#include <kernel/thread.h>
#include <kernel/scheduler.h>
#include <kernel/mem.h>

#include <stdint.h>

typedef struct params {
  uint32_t gs;
  uint32_t fs;
  uint32_t es;
  uint32_t ds;
  
  uint32_t edi;
  uint32_t esi;
  uint32_t ebp;
  uint32_t esp;
  uint32_t ebx;
  uint32_t edx;
  uint32_t ecx;
  uint32_t eax;
} __attribute__((packed)) params_t;

void isr_routine_80(void* isr_stack, void *stack) {
  // standard system call.
  (void)(isr_stack);
  params_t *params = (params_t*)stack;

  switch(params->eax) {
  case SYSCALL_PRINT:
    // TODO: limit string length, don't write out of screen.
    kprintf((const char*)params->edx);
    break;
  case SYSCALL_ALLOC:
    // TODO: memory must by contained within thread address space.
    mem_alloc_mapped((void*)params->edx, params->ecx);
    break;
  case SYSCALL_EXIT:
    kprintf("Exit: %x\n", (unsigned)params->edx);
    CURRENT_THREAD->state = THREAD_STATE_DEAD;
    break;
  default:
    kprintf("unsupported isr_routine_80: eax=%x\n", (unsigned)params->eax);
    kabort();
  }

  scheduler_goto_next();
}

void isr_routine_81() {
  // interrupt request completed.
  //kprintf("isr_routine_81: %u\n", CURRENT_THREAD->pic_line);
  if (CURRENT_THREAD->pic_line) {
    isr_pic_eoi(CURRENT_THREAD->pic_line - 1);
    CURRENT_THREAD->pic_line = 0;
    CURRENT_THREAD->state = THREAD_STATE_DEAD;
    pic_isr_process_next();
    
  } else {
    kprintf("Non-interrupt thread called IRET!\n");
    kabort();
  }
}
