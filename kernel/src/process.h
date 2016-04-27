#ifndef __HELLBENDER_KERNEL_PROCESS_H__
#define __HELLBENDER_KERNEL_PROCESS_H__

#include "spin.h"
#include "list.h"
#include "pqueue.h"
#include "fifo.h"
#include "signal.h"

#include <stdint.h>
#include <stddef.h>

struct thread;

// Describes a memory area for a new process.
struct process_memory {
  uint64_t  flags;  // PAGE_ flags.
  uintptr_t v_base; // base virtual address into which this memory block will be mapped.
  size_t    v_size; // size of the virtual memory block; rounded up to next page.
  void*     m_base; // page aligned memory for this block. must be compatible with flags.
  intptr_t  m_size; // size of the physical memory; if negative memory will be mapped to the top.
};

/* Description of a new process to create.
 * 
 * allocate sizeof(struct process_descriptor) + (N-1)*sizeof(struct process_memory)
 */
struct process_descriptor {
  uintptr_t vmem_base; // limits on process virtual memory area.
  uintptr_t vmem_size;
  uintptr_t entry_point;
  uintptr_t stack_bottom; // lowest valid stack address.
  uintptr_t stack_top;    // stack_bottom + size of stack.
  uintptr_t local_bottom; // address for thread local initialization memory.
  size_t local_size;      // size of thread local memory.
  unsigned n_maps;
  struct process_memory memory_maps[1];
};

/* An actual, existing process.
 */
struct process {
  union {
    struct {
      spinlock_raw_t lock;
      uint64_t pid;
      uint64_t pcid;
      uint64_t *pdpt; // page directory pointer table for the process.
      uintptr_t vmem_base; // limits on process virtual memory area.
      uintptr_t vmem_size;
      unsigned usermode; // 1 for user mode process; 0 for service process.
      
      list_t threads; // struct thread::process_threads.
    };
    uint8_t dummy[CACHE_LINE];
  };

  // these fields are to be touched only by signal_* functions in signal.c
  union process_signal {
    struct {
      spinlock_raw_t lock;      // you may lock thread.signal.lock while holding this lock.
      uint64_t ignore_mask;     // bit set => signal ignored.
      uint64_t default_mask;    // bit set => default action.
      uint64_t pending_mask;    // bit set => signal is pending in sig[x].pending fifo.
      list_t waiting_list;      // thread.signal.waiting_item; threads waiting some signal.
      struct process_signal_info {
        struct thread* waiting; // a thread that may be currently waiting this signal.
        fifo_t pending;         // struct signal_pending (in signal.c).
        //void* action;           // the sigaction function, or 0 if default action.
      } sig[SIGNAL_LIMIT];
    };
    uint8_t dummy[CACHE_LINE];
  } signal;

  // these fields are to be touched only by scheduler_* functions in scheduler.c
  union process_scheduler {
    struct {
      unsigned priority;
      pqueue_t runnable_threads;   // thread.scheduler.runnable_item
      pqueue_item_t runnable_item; // scheduler.runnable_processes
    };
    uint8_t dummy[CACHE_LINE];
  } scheduler;
};

struct process_descriptor* process_alloc_descriptor(unsigned n_maps);

struct process* process_create(struct process_descriptor* desc);

void process_page_map(struct process* proc, void* virt, uintptr_t phys, uint64_t attrib);

#endif
