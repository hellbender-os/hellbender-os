#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <kernel/kernel.h>
#include <kernel/syscall.h>
#include <kernel/kstdio.h>
#include <kernel/kstdlib.h>

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
  (void)(isr_stack);
  params_t *params = (params_t*)stack;

  switch(params->eax) {
  case SYSCALL_PRINT:
    BREAK;
    kprintf((const char*)params->edx);
    break;
  case SYSCALL_EXIT:
    kprintf("Exit: %x\n", (unsigned)params->edx);
    kabort();
    break;
  default:
    kprintf("unsupported isr_routine_80: eax=%x\n", (unsigned)params->eax);
    kabort();
  }
  

  BREAK;
  kernel_to_usermode();
}

void isr_routine_81() {
  kprintf("isr_routine_81\n");
  kabort();
}

void isr_routine_82() {
  kprintf("isr_routine_82\n");
  kabort();
}

void isr_routine_83() {
  kprintf("isr_routine_83\n");
  kabort();
}

void isr_routine_84() {
  kprintf("isr_routine_84\n");
  kabort();
}

void isr_routine_85() {
  kprintf("isr_routine_85\n");
  kabort();
}

void isr_routine_86() {
  kprintf("isr_routine_86\n");
  kabort();
}

void isr_routine_87() {
  kprintf("isr_routine_87\n");
  kabort();
}
