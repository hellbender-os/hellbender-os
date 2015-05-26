#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <kernel/kernel.h>
#include <kernel/process.h>
#include <kernel/thread.h>
#include <kernel/scheduler.h>

typedef struct scheduler {
  thread_t* realtime;
  thread_t* threads[16];
  unsigned num_threads;
  unsigned next_thread;
} scheduler_t;

scheduler_t scheduler;

void scheduler_initialize() {
  memset(&scheduler, 0, sizeof(scheduler));
}

void scheduler_set_realtime_thread(thread_t *thread) {
  scheduler.realtime = thread;
}

void scheduler_add_thread(thread_t *thread) {
  if (scheduler.num_threads >= 16) {
    printf("Max number of threads reached.\n");
    abort();
  }
  scheduler.threads[scheduler.num_threads++] = thread;
}

__attribute__((__noreturn__))
void scheduler_goto_next() {
  thread_t *thread = NULL;
  if (scheduler.realtime && scheduler.realtime->state != THREAD_STATE_DEAD
      && scheduler.realtime->state != THREAD_STATE_WAIT) {
    thread = scheduler.realtime;
  }
  for (unsigned i = 0; thread == NULL && i < scheduler.num_threads; ++i) {
    if (++scheduler.next_thread >= scheduler.num_threads) {
      scheduler.next_thread = 0;
    }
    thread_t *this = scheduler.threads[scheduler.next_thread];
    if (this->state == THREAD_STATE_WAIT) {
      switch (this->wait_state.op) {
      case THREAD_WAIT_EQ:
        if (*this->wait_state.obj == this->wait_state.value) {
          this->state = THREAD_STATE_OLD;
        }
        break;
      case THREAD_WAIT_NEQ:
        if (*this->wait_state.obj != this->wait_state.value) {
          this->state = THREAD_STATE_OLD;
        }
        break;
      }
    }
    if (this->state != THREAD_STATE_DEAD && this->state != THREAD_STATE_WAIT) {
      thread = this;
      break;
    }
  }
  if (thread) {
    if (thread != kernel.current_thread) thread_set_current(thread);
    kernel_to_usermode();
  } else {
    kernel_halt();
  }
}

void scheduler_make_wait(thread_t *thread) {
  thread->state = THREAD_STATE_WAIT;
}
