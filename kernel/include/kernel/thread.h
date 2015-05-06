#ifndef _KERNEL_THREAD_H
#define _KERNEL_THREAD_H

#include <stddef.h>

// current thread page table is mapped into this address.
#define THREAD_OFFSET 0x800000

// NEW for just created thread, OLD for threads that have been running.
#define THREAD_STATE_NEW  0xA11B00b5
#define THREAD_STATE_OLD  0xC001D00d
#define THREAD_STATE_DEAD 0xDeadBeef

// each thread has a page table.
// the page table is mapped to THREAD_OFFSET when the thread is running.
// this structure is mapped as the first page of the page table.
// kernel thread is mapped as the second page of the page table.
// thread stack is mapped at the top of the page table.
//
// this structure is also mapped into kernel memory at thread_id*PAGE_SIZE.
//
// when thread is created, state == _NEW.
// after thread has been running, state == _OLD.
// CPU registers are restored on context switch only when state == _OLD.
typedef struct thread {
  void* kernel_stack_ptr; // current pointer of the kernel stack. MUST BE FIRST!

  uint32_t thread_id; // unique identifier.
  uint32_t state; // new or old, depending if thread has been running.
  void* start_address; // start address for new threads.
  unsigned pic_line; // one based PIC line number; for PIC interrupt handlers.

  struct thread *this; // logical address of this structure in kernel memory.
  uintptr_t page_table; // physical address to the thread page table.
  
  void* stack_bottom; // range that contains stack; top of the address space.
  void* stack_top;
  
} thread_t;

#define CURRENT_THREAD ((thread_t*)THREAD_OFFSET)

thread_t* thread_allocate(void *start_address);
void thread_set_current(thread_t* thread);

void* thread_grow_stack(thread_t *thread, size_t size); // returns new bottom.

#endif
