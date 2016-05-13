#include "signal.h"
#include "process.h"
#include "thread.h"
#include "heap.h"
#include "scheduler.h"
#include "kernel.h"

struct signal_pending {
  fifo_item_t item;
  intptr_t sigval;
};

struct signal {
  uint64_t seq, test;
} signal;


void signal_set_mask(struct thread* thread, uint64_t blocked_mask) {
  SPIN_GUARD_RAW(thread->signal.lock);
  thread->signal.blocked_mask = blocked_mask;
  ++signal.seq;
}

void signal_process(struct process* process, int signum, intptr_t sigval) {
  struct thread* delivered = 0; // thread to awaken.
  {
    SPIN_GUARD_RAW(process->signal.lock);
    ++signal.seq;
    uint64_t sigbit = 1ull << (signum%SIGNAL_LIMIT);
  
    // can we do 'instant delivery' to the signal specific waiting thread?
    struct process_signal_info* sig = process->signal.sig + signum;
    struct thread* thread = sig->waiting;
    if (thread) {
      SPIN_GUARD_RAW(thread->signal.lock);
      if (thread->signal.wait_mask & sigbit) {
        // deliver the signal to this thread.
        thread->signal.wait_mask = 0;
        thread->signal.wait_signum = signum;
        thread->signal.wait_sigval = sigval;
        delivered = thread;
        // remove it from waiting list.
        sig->waiting = 0;
        list_remove(&thread->signal.waiting_item);
      }
    }

    // can we do 'instant delivery' to any waiting thread?
    if (!delivered) {
      for (list_item_t *li = list_first(&process->signal.waiting_list); 
           li; li = list_next(li)) {
        struct thread* thread = list_container(li, struct thread, signal.waiting_item);
        SPIN_GUARD_RAW(thread->signal.lock);
        if (thread->signal.wait_mask & sigbit) {
          // deliver the signal to this thread.
          thread->signal.wait_mask = 0;
          thread->signal.wait_signum = signum;
          thread->signal.wait_sigval = sigval;
          delivered = thread;
          // remove it from waiting list.
          list_remove(li);
          break;
        }        
      }
    }

    // queue the signal if it was not instantly delivered.
    if (!delivered) {
      struct signal_pending *pending = HEAP_NEW(struct signal_pending);
      pending->sigval = sigval;
      fifo_push(&sig->pending, &pending->item);
      process->signal.pending_mask |= sigbit;
    }
  }
  
  // wake up the thread that received the signal:
  if (delivered) {
    scheduler_wakeup(delivered);
  }
}

void signal_thread(struct thread* thread, int signum, intptr_t sigval) {
  struct thread* delivered = 0; // thread to awaken.
  uint64_t sigbit = 1ull << (signum%SIGNAL_LIMIT);
  {
    SPIN_GUARD_RAW(thread->signal.lock);
    ++signal.seq;
    // can we do 'instant delivery'?
    if (thread->signal.wait_mask & sigbit) {
      // deliver the signal to this thread.
      thread->signal.wait_mask = 0;
      thread->signal.wait_signum = signum;
      thread->signal.wait_sigval = sigval;
      delivered = thread;
      // remove it from waiting list.
      list_remove(&thread->signal.waiting_item);
    }

    // queue the signal if it was not instantly delivered.
    if (!delivered) {
      struct thread_signal_info* sig = thread->signal.sig + signum;
      struct signal_pending *pending = HEAP_NEW(struct signal_pending);
      pending->sigval = sigval;
      fifo_push(&sig->pending, &pending->item);
      thread->signal.pending_mask |= sigbit;
    }
  }
  
  // wake up the thread if it received the signal:
  if (delivered) {
    scheduler_wakeup(delivered);
  }
}

int signal_wait(struct thread* thread, uint64_t wait_mask) {
  int retval = 0;
  struct thread *sleeping = 0; // set if thread should go to sleep.
  {
    struct process* process = thread->process;
    SPIN_GUARD_RAW(process->signal.lock);
    SPIN_GUARD_RAW(thread->signal.lock);
    
    // is a waited signal already pending?
    int process_signum = __builtin_ffsll(process->signal.pending_mask & wait_mask);
    int thread_signum = __builtin_ffsll(thread->signal.pending_mask & wait_mask);
    if (process_signum && (!thread_signum || process_signum < thread_signum)) {
      int signum = process_signum - 1;
      uint64_t sigbit = 1ull << (signum%SIGNAL_LIMIT);
      struct process_signal_info* sig = process->signal.sig + signum;
      fifo_item_t *fi = fifo_pop(&sig->pending);
      if (fifo_empty(&sig->pending)) process->signal.pending_mask &= ~sigbit;
      struct signal_pending* pending = fifo_container(fi, struct signal_pending, item);
      thread->signal.wait_mask = 0;
      thread->signal.wait_signum = retval = signum;
      thread->signal.wait_sigval = pending->sigval;
      heap_free(pending);
    }
    else if (thread_signum) {
      int signum = thread_signum - 1;
      uint64_t sigbit = 1ull << (signum%SIGNAL_LIMIT);
      struct thread_signal_info* sig = thread->signal.sig + signum;
      fifo_item_t *fi = fifo_pop(&sig->pending);
      if (fifo_empty(&sig->pending)) thread->signal.pending_mask &= ~sigbit;
      struct signal_pending* pending = fifo_container(fi, struct signal_pending, item);
      thread->signal.wait_mask = 0;
      thread->signal.wait_signum = retval = signum;
      thread->signal.wait_sigval = pending->sigval;
      heap_free(pending);
    }
    else { // the thread must sleep until a signal arrives.
      thread->signal.wait_signum = 0;
      thread->signal.wait_mask = wait_mask;
      sleeping = thread;
      list_insert(&process->signal.waiting_list, &thread->signal.waiting_item);
      for (uint64_t mask = wait_mask; mask; ) {
        int signum = __builtin_ffsll(mask) - 1;
        struct process_signal_info* sig = process->signal.sig + signum;
        sig->waiting = thread;
        mask &= ~(1ull << signum%64);
      }
    }
  }

  // put the thread to sleep if there were no pending signals.
  // conditional sleep avoid races with arriving signals.
  if (sleeping) {
    scheduler_sleep_conditionally(thread, &thread->signal.wait_mask, wait_mask);
  }
  return retval;
}

void signal_delivery() {
  struct thread* thread = cpu.current_thread;
  struct process* process = thread->process;

  struct signal_pending* pending = 0;
  int signum = 0;
  uint64_t sigbit = 0;

  SPIN_GUARD_RAW(process->signal.lock);
  SPIN_GUARD_RAW(thread->signal.lock);
  if (signal.seq == signal.test) return; // nothing has chaged since we last checked.
  signal.test = signal.seq;

  KERNEL_ASSERT(thread->signal.wait_mask == 0);
  uint64_t accept_mask = ~(thread->signal.blocked_mask | thread->signal.delivery_mask);
  
  // is there a signal that could be delivered?
  int process_signum = __builtin_ffsll(process->signal.pending_mask & accept_mask);
  int thread_signum = __builtin_ffsll(thread->signal.pending_mask & accept_mask);
  if (process_signum && (!thread_signum || process_signum < thread_signum)) {
    signum = process_signum - 1;
    sigbit = 1ull << (signum%SIGNAL_LIMIT);
    struct process_signal_info* sig = process->signal.sig + signum;
    fifo_item_t *fi = fifo_pop(&sig->pending);
    if (fifo_empty(&sig->pending)) process->signal.pending_mask &= ~sigbit;
    pending = fifo_container(fi, struct signal_pending, item);
  }
  else if (thread_signum) {
    signum = thread_signum - 1;
    sigbit = 1ull << (signum%SIGNAL_LIMIT);
    struct thread_signal_info* sig = thread->signal.sig + signum;
    fifo_item_t *fi = fifo_pop(&sig->pending);
    if (fifo_empty(&sig->pending)) thread->signal.pending_mask &= ~sigbit;
    pending = fifo_container(fi, struct signal_pending, item);
  }

  // deliver the signal.
  if (signum) {
    if (process->signal.ignore_mask & sigbit) {
      // ignored signal.
    } 
    else if (process->signal.default_mask & sigbit) {
      // default action.
      switch (signum) {
      case 1: break;
      default:
        // TODO: terminate program.
        break;
      }
    } 
    else {
      // TODO: run process->signal.sig[signum].action function.
      // thread->delivery_mask |= signum.
    }
    heap_free(pending);
  }
}
