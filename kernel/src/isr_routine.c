#include "isr.h"
#include "thread.h"
#include "service.h"
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
  // TODO: check if NX problem,
  //  if source at IDC area, idc_call.
  if (state->context.rip >= SERVICE_TABLE_BASE
      && state->context.rip < (SERVICE_TABLE_BASE + SERVICE_TABLE_SIZE)) {
    service_call(state->context.rip - SERVICE_TABLE_BASE);
  }
  //  if source at process area && in_idc, idc_return.
  // else kernel_panic
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
