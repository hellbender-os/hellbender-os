#include "scheduler.h"
#include "process.h"
#include "thread.h"
#include "spin.h"
#include "pqueue.h"
#include "kernel.h"
#include "cpu.h"

struct scheduler {
  spinlock_raw_t lock;      // you may lock process.scheduler.lock while holding this lock.
  uint64_t seq_nr, test_nr; // scheduling run only when seq_nr > test_nr.
  pqueue_t runnable_processes; // process.scheduler.runnable_item; process.scheduler.runnable_threads must not be empty.
};

static struct scheduler scheduler;

INLINE int scheduler_wait() {
  // nothing to run, just sit tight.
  asm volatile("sti;hlt;cli");
  return 1;
}

uint64_t scheduler_run(uint64_t ret_address) {
  if (cpu.current_thread) cpu.current_thread->ret_address = ret_address;

  do { // we cannot return until we have something to run in usermode.
    struct process *best_p = 0;
    struct thread *best_t = 0;
    switch (1) { case 1:; // just so that it is easy to break once suitable thread is found.
      SPIN_GUARD_RAW(scheduler.lock);
      // no need to test if nothing has changed.
      if (scheduler.seq_nr == scheduler.test_nr) {
        best_p = cpu.current_process;
        best_t = cpu.current_thread;
        break;
      }
      scheduler.test_nr = scheduler.seq_nr;

      // compare priorities of currently running and highest runnable processes.
      // 0 means not available and any valid priority > 0.
      unsigned curr_ppri = 0;
      unsigned curr_tpri = 0;
      unsigned next_ppri = pqueue_priority(&scheduler.runnable_processes);
      unsigned next_tpri = 0;
      if (cpu.current_process && cpu.current_thread) {
        curr_ppri = cpu.current_process->scheduler.priority;
        if (curr_ppri >= next_ppri) {
          // current process not worse than the best runner-up, just check if the current
          // process still has runnable threads:
          if (cpu.current_thread->scheduler.state == SCHEDULER_STATE_RUNNABLE) {
            curr_tpri = cpu.current_thread->scheduler.priority;
          }
          next_tpri = pqueue_priority(&cpu.current_process->scheduler.runnable_threads);
          if (curr_tpri && curr_tpri >= next_tpri) {
            // keep running the same thread.
            best_p = cpu.current_process;
            best_t = cpu.current_thread;
            break;
          } else if (next_tpri) {
            // switch to another thread in the same process.
            if (curr_tpri) {
              pqueue_push(&cpu.current_process->scheduler.runnable_threads,
                          &cpu.current_thread->scheduler.runnable_item,
                          cpu.current_thread->scheduler.priority);
            }
            best_p = cpu.current_process;
            pqueue_item_t *item = pqueue_pop(&best_p->scheduler.runnable_threads);
            best_t = pqueue_container(item, struct thread, scheduler.runnable_item);
            break;
          } else {
            // current thread no longer runnable.
            curr_ppri = 0;
          }
        }
      }
      if (next_ppri > curr_ppri) {
        // switch to another process, but save the old state first.
        if (cpu.current_thread 
            && cpu.current_thread->scheduler.state == SCHEDULER_STATE_RUNNABLE) {
          pqueue_push(&cpu.current_process->scheduler.runnable_threads,
                      &cpu.current_thread->scheduler.runnable_item,
                      cpu.current_thread->scheduler.priority);
        }
        if (cpu.current_process 
            && pqueue_priority(&cpu.current_process->scheduler.runnable_threads)) {
          pqueue_push(&scheduler.runnable_processes,
                      &cpu.current_process->scheduler.runnable_item,
                      cpu.current_process->scheduler.priority);
        }
        pqueue_item_t *pitem = pqueue_pop(&scheduler.runnable_processes);
        best_p = pqueue_container(pitem, struct process, scheduler.runnable_item);
        pqueue_item_t *titem = pqueue_pop(&best_p->scheduler.runnable_threads);
        best_t = pqueue_container(titem, struct thread, scheduler.runnable_item);
        break;
      }
    }

    // start running the best thread.
    if (best_t) {
      cpu_set_thread(best_t);
    } else {
      cpu_set_process(0);
      cpu_set_thread(0);
    }
  } while (cpu.current_thread == 0 && scheduler_wait());

  return cpu.current_thread->ret_address;
}

void scheduler_wakeup(struct thread* thread) {
  SPIN_GUARD_RAW(scheduler.lock);
  struct process *process = thread->process;
  if (thread->scheduler.state == SCHEDULER_STATE_WAITING) {
    ++scheduler.seq_nr;
    thread->scheduler.state = SCHEDULER_STATE_RUNNABLE;
    if (pqueue_empty(&process->scheduler.runnable_threads)) {
      pqueue_push(&scheduler.runnable_processes,
                  &thread->process->scheduler.runnable_item,
                  thread->process->scheduler.priority);
    }
    pqueue_push(&process->scheduler.runnable_threads, 
                &thread->scheduler.runnable_item,
                thread->scheduler.priority);
  }
}

void scheduler_sleep_conditionally(struct thread* thread, uint64_t* ptr, uint64_t value) {
  SPIN_GUARD_RAW(scheduler.lock);
  struct process *process = thread->process;
  if (thread->scheduler.state == SCHEDULER_STATE_RUNNABLE && *ptr == value) {
    ++scheduler.seq_nr;
    thread->scheduler.state = SCHEDULER_STATE_WAITING;
    pqueue_remove(&process->scheduler.runnable_threads,
                  &thread->scheduler.runnable_item, 
                  thread->scheduler.priority);
    if (pqueue_empty(&process->scheduler.runnable_threads)) {
      pqueue_remove(&scheduler.runnable_processes,
                    &process->scheduler.runnable_item,
                    process->scheduler.priority);
    }
  }
}

void scheduler_set_process_priority(struct process* process, unsigned priority) {
  SPIN_GUARD_RAW(scheduler.lock);
  ++scheduler.seq_nr;
  if (priority != process->scheduler.priority) {
    if (!pqueue_empty(&process->scheduler.runnable_threads)) {
      pqueue_remove(&scheduler.runnable_processes,
                    &process->scheduler.runnable_item,
                    process->scheduler.priority);
      process->scheduler.priority = priority;
      pqueue_push(&scheduler.runnable_processes,
                  &process->scheduler.runnable_item,
                  process->scheduler.priority);
    }
  }
}

void scheduler_set_thread_priority(struct thread* thread, unsigned priority) {
  SPIN_GUARD_RAW(scheduler.lock);
  ++scheduler.seq_nr;
  if (priority != thread->scheduler.priority) {
    if (thread->scheduler.state == SCHEDULER_STATE_RUNNABLE) {
      pqueue_remove(&thread->process->scheduler.runnable_threads,
                    &thread->scheduler.runnable_item, 
                    thread->scheduler.priority);
      thread->scheduler.priority = priority;
      pqueue_push(&thread->process->scheduler.runnable_threads, 
                  &thread->scheduler.runnable_item,
                  thread->scheduler.priority);
    }
  }
}
