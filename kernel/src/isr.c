#include "isr.h"
#include "process.h"
#include "spin.h"
#include "heap.h"

#include <hellbender/list.h>

struct isr_item {
  list_item_t item;
  struct process* process;
  int signum;
};

struct isr_signal {
  spinlock_raw_t lock; // protects irq[x].list
  struct irq {
    union {
      struct {
        spinlock_raw_t lock; // protects process, signum.
        list_t list;
        struct process* process;
        int signum;
      };
      uint8_t dummy[CACHE_LINE];
    };
  } irq[IRQ_LIMIT];
};

static struct isr_signal isr_signal;


// only one process can receive any single interrupt.
void isr_set_signal(unsigned irq_nr, struct process *process, int signum) {
  struct irq* irq = isr_signal.irq + irq_nr;
  SPIN_GUARD_RAW(isr_signal.lock);
  struct isr_item *si = HEAP_NEW(struct isr_item);
  si->process = process;
  si->signum = signum;
  list_insert(&irq->list, &si->item);
  {
    SPIN_GUARD_RAW(irq->lock);
    irq->process = process;
    irq->signum = signum;
  }
}

void isr_clear_signal(unsigned irq_nr, struct process *process) {
  struct irq* irq = isr_signal.irq + irq_nr;
  int signum = 0;
  SPIN_GUARD_RAW(isr_signal.lock);
  for (list_item_t *li = list_first(&irq->list); li;) {
    struct isr_item *item =  list_container(li, struct isr_item, item);
    if (item->process == process) {
      li = list_remove(li);
      heap_free(item);
    } else {
      li = list_next(li);
    }
  }
  list_item_t *li = list_first(&irq->list);
  if (li) {
    struct isr_item *item = list_container(li, struct isr_item, item);
    process = item->process;
    signum = item->signum;
  } else {
    process = 0;
    signum = 0;
  }
  {
    SPIN_GUARD_RAW(irq->lock);
    irq->process = process;
    irq->signum = signum;
  }
}

void isr_send_signal(unsigned irq_nr) {
  // get the process that should receive the signal, while holding the lock.
  struct irq* irq = isr_signal.irq + irq_nr;
  struct process* process = 0;
  int signum = 0;
  {
    SPIN_GUARD_RAW(irq->lock);
    process = irq->process;
    signum = irq->signum;
  }

  // send the signal while not holding the lock.
  if (process) {
    signal_process(process, signum, irq_nr);
  }
}
