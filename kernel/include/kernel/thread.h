#ifndef _KERNEL_THREAD_H
#define _KERNEL_THREAD_H

#include <stdint.h>

#include <kernel/kernel.h>
#include <kernel/domain.h>

// NEW for just created thread, OLD for threads that have been running.
#define THREAD_STATE_NEW  0xA11B00b5
#define THREAD_STATE_OLD  0xC001D00d
#define THREAD_STATE_WAIT 0xF00dBabe
#define THREAD_STATE_DEAD 0xDeadBeef

typedef unsigned wait_result_t;
#define WAIT_NO_MORE 0
#define WAIT_STILL 1

typedef struct wait_state wait_state_t;
typedef wait_result_t (*wait_func_ptr)(wait_state_t* state);

typedef struct wait_state {
  wait_func_ptr wait_func;
  void* wait_obj;
  unsigned seq_nr;
} wait_state_t;


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
  unsigned is_kernel; // true, if the thread runs using kernel selectors.

  uint32_t state; // new or old, depending if thread has been running.
  wait_state_t wait_state;

  char* exec_path; // path to the binary that is beign executed.
  uintptr_t exec_args; // physical address where argv, envp are stored.
  unsigned exec_envp; // index into exec_args where environ variables begin.
  
  void* start_address; // start address for new threads.
  unsigned pic_line; // one based PIC line number; for PIC interrupt handlers.

  struct thread *this; // logical address of this structure in kernel memory.
  uintptr_t page_table; // physical address to the thread page table.
  
  void* stack_bottom; // range that contains stack; top of the address space.
  void* stack_top;

  domain_t *home_domain;
  void* heap_bottom;
  void* heap_limit;

  struct domain_stack {
    uintptr_t entry_address;
    uintptr_t return_address;
  } domain_stack[IDC_CALL_MAX];
  size_t domain_idx;
  
} thread_t;

typedef struct thread_state {
  uint32_t gs;
  uint32_t fs;
  uint32_t es;
  uint32_t ds;
  
  uint32_t edi;
  uint32_t esi;
  uint32_t ebp;
  uint32_t esp;
  uint32_t ebx;
  uint32_t edx;
  uint32_t ecx;
  uint32_t eax;
} __attribute__((packed)) thread_state_t;

#if defined(__is_hellbender_kernel)
// kernel has full access to thread.
#define CURRENT_THREAD ((thread_t*)THREAD_OFFSET)
#define CURRENT_STATE ((thread_state_t*)(CURRENT_THREAD->kernel_stack_ptr))

#else
// ring3 has only read only access to thread info.
#define CURRENT_THREAD ((thread_t*)(THREAD_OFFSET + 2*PAGE_SIZE))

#endif

void thread_initialize();
thread_t* thread_create(domain_t *home_domain, void *start_address);
void thread_set_current(thread_t* thread);

void* thread_grow_stack(thread_t *thread, size_t size); // returns new bottom.

#endif
