#ifndef __HELLBENDER_KERNEL_PROCESS_H__
#define __HELLBENDER_KERNEL_PROCESS_H__

#include "spin.h"

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
  uintptr_t entry_point;
  uintptr_t stack_bottom; // lowest valid stack address.
  uintptr_t stack_top;    // stack_bottom + size of stack.
  uintptr_t local_bottom; // address for thread local initialization memory.
  size_t local_size;      // size of thread local memory.
  unsigned n_maps;
  struct process_memory memory_maps[1];
};

//TODO
#define PROCESS_MAX_THREADS 16

/* An actual, existing process.
 */
struct process {
  spinlock_raw_t lock;
  uint64_t pid;
  uint64_t pcid;
  uint64_t *pdpt; // page directory pointer table for the process.

  struct thread* threads[PROCESS_MAX_THREADS];
  unsigned n_threads;
};

struct process_descriptor* process_alloc_descriptor(unsigned n_maps);

struct process* process_create(struct process_descriptor* desc);

void process_page_map(struct process* proc, void* virt, uintptr_t phys, uint64_t attrib);

#endif
