#include "thread.h"
#include "process.h"
#include "config.h"
#include "lomem.h"
#include "page.h"
#include "heap.h"
#include "scheduler.h"

#include <string.h>

static uint64_t thread_next_id = 0;

struct thread* thread_create(struct process* process,
                             uintptr_t entry_point, uintptr_t stack_top,
                             uintptr_t local_bottom, size_t local_size) {
  // allocate a new thread.
  struct thread* t = heap_alloc(sizeof(struct thread*));
  memset(t, 0, sizeof(struct thread));
  t->tid = ++thread_next_id;
  t->process = process;
  t->stack_top = kernel_p2v(page_clear(lomem_alloc_4k()) + PAGE_SIZE);
  t->scheduler.priority = SCHEDULER_PRIORITY_NORMAL;

  // allocate thread locals.
  t->thread_local_pages = page_round_up(local_size) / PAGE_SIZE;
  if (local_size) {
    uintptr_t *local_pt = kernel_p2v(page_clear(lomem_alloc_4k()));
    t->thread_local_pt = local_pt;
    for (size_t i = 0; i < t->thread_local_pages; --i, ++local_pt) {
      *local_pt = page_clear(lomem_alloc_4k());
    }
  }

  // initialize state.
  t->rsp_backup = (uintptr_t)t->stack_top - sizeof(struct thread_state);
  struct thread_state *state = (struct thread_state*)t->rsp_backup;
  state->context.rip = entry_point;
  state->context.cs = UCODE_SELECTOR;
  state->context.rflags = 0x200; // IF enabled.
  state->context.rsp = stack_top;
  state->context.ss = UDATA_SELECTOR;
  // by convention: registers rdi, rsi, rdx define the thread local init memory.
  state->registers.rdi = THREAD_LOCAL_BASE;
  state->registers.rsi = local_bottom;
  state->registers.rdx = local_size;
  // by convention r8, r9 describe the process / thread.
  state->registers.rcx = 0;
  state->registers.r8 = process->pid;
  state->registers.r9 = t->tid;

  return t;
}

void thread_local_invalidate(size_t pages) {
  // invalidate any thread local pages from the TLB.
  for (size_t i = 0; i < pages; ++i) {
    page_invalidate((void*)(THREAD_LOCAL_BASE + i * PAGE_SIZE));
  }
}
