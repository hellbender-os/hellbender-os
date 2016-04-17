#include "thread.h"
#include "lomem.h"
#include "page.h"
#include "heap.h"

#include <string.h>

struct thread* create_thread(uintptr_t entry_point, uintptr_t stack_top) {
  // allocate a new thread.
  struct thread* t = heap_alloc(sizeof(struct thread*));
  memset(t, 0, sizeof(struct thread));
  t->stack_top = kernel_p2v(page_clear(lomem_alloc_4k()));

  // initialize state.
  t->rsp_backup = (uintptr_t)t->stack_top - sizeof(struct thread_state);
  struct thread_state *state = (struct thread_state*)t->rsp_backup;
  state->context.rip = entry_point;
  state->context.cs = UCODE_SELECTOR;
  state->context.rflags = 0x200; // IF enabled.
  state->context.rsp = stack_top;
  state->context.ss = UDATA_SELECTOR;

  return t;
}

