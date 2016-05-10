#include "isr.h"
#include "cpu.h"
#include "kernel.h"
#include "page.h"
#include "thread.h"
#include "process.h"
#include "service.h"
#include "log.h"

#include <hellbender/debug.h>

// traps

void isr_routine_0(struct thread_state* state, unsigned ring) {
  (void)state;
  (void)ring;
}

void isr_routine_1(struct thread_state* state, unsigned ring) {
  (void)state;
  (void)ring;
}

void isr_routine_2(struct thread_state* state, unsigned ring) {
  (void)state;
  (void)ring;
}

void isr_routine_3(struct thread_state* state, unsigned ring) {
  (void)state;
  (void)ring;
}

void isr_routine_4(struct thread_state* state, unsigned ring) {
  (void)state;
  (void)ring;
}

void isr_routine_5(struct thread_state* state, unsigned ring) {
  (void)state;
  (void)ring;
}

void isr_routine_6(struct thread_state* state, unsigned ring) {
  (void)state;
  (void)ring;
}

void isr_routine_7(struct thread_state* state, unsigned ring) {
  (void)state;
  (void)ring;
}

void isr_routine_8(struct thread_state* state, unsigned ring) {
  (void)state;
  (void)ring;
}

void isr_routine_9(struct thread_state* state, unsigned ring) {
  (void)state;
  (void)ring;
}

void isr_routine_10(struct thread_state* state, unsigned ring) {
  (void)state;
  (void)ring;
}

void isr_routine_11(struct thread_state* state, unsigned ring) {
  (void)state;
  (void)ring;
}

void isr_routine_12(struct thread_state* state, unsigned ring) {
  (void)state;
  (void)ring;
}

void isr_routine_13(struct thread_state* state, unsigned ring) {
  (void)state;
  (void)ring;
}

// page fault.
void isr_routine_14(struct thread_state* state, unsigned ring) {
  (void)state;
  (void)ring;
  void* ptr;
  struct process_vmem *vmem;
  asm volatile("mov %%cr2, %0" : "=r"(ptr));

  uint64_t rip = state->context.rip;
  uint64_t error = state->context.error;
  int present = error & 1;
  int write = error & 2;
  int user = error & 4;
  //int reserved = error & 8;
  int fetch = error & 16;

  // if instruction fetch at IDC area => idc_call.
  if (fetch && rip >= SERVICE_TABLE_BASE && rip < (SERVICE_TABLE_BASE + SERVICE_TABLE_SIZE)) {
    service_call(state->context.rip - SERVICE_TABLE_BASE);
  }
  // TODO: if source at process area && in_idc, idc_return.

  // if write, check if it is in CoW area.
  else if (present && user && write && (vmem = process_get_vmem(cpu.current_process, ptr))
           && vmem->flags & PROCESS_VMEM_SEMI_COW) {
    page_copy_on_write(ptr);
  }
  else { 
    log_error("isr", "page_fault", "Unknown page fault");
  }
  // TODO do what page fault does.
}

void isr_routine_15(struct thread_state* state, unsigned ring) {
  (void)state;
  (void)ring;
}

void isr_routine_16(struct thread_state* state, unsigned ring) {
  (void)state;
  (void)ring;
}

void isr_routine_17(struct thread_state* state, unsigned ring) {
  (void)state;
  (void)ring;
}

void isr_routine_18(struct thread_state* state, unsigned ring) {
  (void)state;
  (void)ring;
}

void isr_routine_19(struct thread_state* state, unsigned ring) {
  (void)state;
  (void)ring;
}

void isr_routine_20(struct thread_state* state, unsigned ring) {
  (void)state;
  (void)ring;
}

void isr_routine_30(struct thread_state* state, unsigned ring) {
  (void)state;
  (void)ring;
}

// PIC interrupts (32-47) handled in pic.c
