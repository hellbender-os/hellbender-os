#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <kernel/kernel.h>
#include <kernel/syscall.h>
#include <kernel/kstdio.h>
#include <kernel/kstdlib.h>
#include <kernel/isr.h>
#include <kernel/thread.h>

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
    kprintf((const char*)params->edx);
    break;
  case SYSCALL_EXIT:
    kprintf("Exit: %x\n", (unsigned)params->edx);
    // TODO: RUN SCHEDULING.
    khalt();
    break;
  default:
    kprintf("unsupported isr_routine_80: eax=%x\n", (unsigned)params->eax);
    kabort();
  }
  
  // TODO: RUN SCHEDULING.
  kernel_to_usermode();
}

void isr_routine_81() {
  // interrupt request completed.
  //kprintf("isr_routine_81: %u\n", CURRENT_THREAD->pic_processor);
  if (CURRENT_THREAD->pic_processor) {
    isr_pic_eoi(CURRENT_THREAD->pic_processor);
    CURRENT_THREAD->pic_processor = 0;
    // TODO: RUN SCHEDULING.
    khalt();
    
  } else {
    kprintf("Non-interrupt thread called IRET!\n");
    kabort();
  }
}
