#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <semaphore.h>

#include <kernel/kernel.h>
#include <kernel/syscall.h>
#include <kernel/isr.h>
#include <kernel/pic_isr.h>
#include <kernel/pic.h>
#include <kernel/thread.h>
#include <kernel/scheduler.h>
#include <kernel/mem.h>
#include <kernel/mmap.h>
#include <kernel/semaphore.h>

#include <stdint.h>

void isr_routine_80(void* isr_stack, thread_state_t *params) {
  // standard system call.
  (void)(isr_stack);

  switch(params->eax) {
  case SYSCALL_PRINT: // edx: const* str
    // TODO: limit string length, don't write out of screen.
    printf((const char*)params->edx);
    break;
  case SYSCALL_ALLOC: // edx: void* ptr; ecx: size_t size
    // TODO: memory must by contained within thread address space.
    mem_alloc_mapped((void*)params->edx, params->ecx, MMAP_ATTRIB_USER_RW);
    break;
  case SYSCALL_EXIT: // edx: int status
    printf("Exit: %x\n", (unsigned)params->edx);
    CURRENT_THREAD->state = THREAD_STATE_DEAD;
    break;
  case SYSCALL_CURRENT_DOMAIN: // edx: domain_t* dst
    // TODO: check permissions.
    if (kernel.current_domain) {
      memcpy((void*)params->edx, kernel.current_domain, sizeof(domain_t));
      params->eax = 1;
    } else {
      memset((void*)params->edx, 0, sizeof(domain_t));
      params->eax = 0;
    }
    break;
  case SYSCALL_YIELD:
    // nothing to do, just re-schedule.
    break;

  case SYSCALL_SEM_OPEN: // edx: sem_t* s; ebx: const char* name; ecx: int oflags
    // TODO: check permissions & validity.
    {
      semaphore_t* ksem = semaphore_find((const char*)params->ebx);
      if (ksem) {
        sem_t* usem = (sem_t*)params->edx;
        usem->id = ksem;
        params->eax = 0;
      } else {
        params->eax = 1;
      }
    }
    break;
  case SYSCALL_SEM_CREATE: // edx: sem_t*; ebx: const char*; ecx: int oflags;
                           // edi: mode_t mode; esi: unsigned value
    // TODO: check permissions & validity.
    {
      semaphore_t* ksem = semaphore_create((const char*)params->ebx,
                                           (unsigned)params->esi);
      if (ksem) {
        sem_t* usem = (sem_t*)params->edx;
        usem->id = ksem;
        params->eax = 0;
      } else {
        params->eax = 1;
      }
    }
    break;
  case SYSCALL_SEM_POST: // edx: sem_t* s
    {
      // TODO: check permissions & validity.
      sem_t* usem = (sem_t*)params->edx;
      semaphore_post((semaphore_t*)usem->id);
      params->eax = 0;
    }
    break;
  case SYSCALL_SEM_WAIT: // edx: sem_t* s
    {
      // TODO: check permissions & validity.
      sem_t* usem = (sem_t*)params->edx;
      semaphore_t* ksem = (semaphore_t*)usem->id;
      params->eax = 0;
      if (semaphore_wait(ksem,
                         &CURRENT_THREAD->wait_state) == WAIT_STILL) {
        scheduler_make_wait(CURRENT_THREAD);
      }
    }
    break;
  default:
    printf("unsupported isr_routine_80: eax=%x\n", (unsigned)params->eax);
    abort();
  }

  scheduler_goto_next();
}

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
