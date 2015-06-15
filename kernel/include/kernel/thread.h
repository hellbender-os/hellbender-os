#ifndef _KERNEL_THREAD_H
#define _KERNEL_THREAD_H

#include <stdint.h>
#include <signal.h>
#include <sys/types.h>

#include <kernel/kernel.h>
#include <kernel/domain.h>

// NEW for just created thread, OLD for threads that have been running.
#define THREAD_STATE_NEW  0xA11B00b5
#define THREAD_STATE_OLD  0xC001D00d
#define THREAD_STATE_WAIT 0xF00dBabe
#define THREAD_STATE_DEAD 0xDeadBeef

#define THREAD_WAIT_EQ  1
#define THREAD_WAIT_NEQ 2

typedef struct thread thread_t;
typedef struct wait_state {
  volatile unsigned *obj;
  unsigned value;
  int op; // 1 == wait for equality, 2 == wait for inequality.
} wait_state_t;


// each thread has a page table.
// the page table is mapped at _thread_base when the thread is running.
// this structure is mapped at _thread_kdata.
// kernel stack is mapped at _thread_kstack .. _thread_kstack_top.
// info structure is mapped at _thread_uinfo.
// local data is mapped at _thread_udata.
// thread stack is mapped at _thread_ustack .. _thread_ustack_top.
//
// this structure is also mapped into kernel memory at thread_id*PAGE_SIZE.
//
// when thread is created, state == _NEW.
// after thread has been running, state == _OLD.
// CPU registers are restored on context switch only when state == _OLD.
typedef struct thread {
  void* kernel_stack_ptr; // current pointer of the kernel stack. MUST BE FIRST!

  pid_t thread_id; // unique identifier.
  unsigned is_kernel; // true, if the thread runs using kernel selectors.

  uint32_t state; // new or old, depending if thread has been running.
  wait_state_t wait_state;
  
  uint64_t time_start;
  uint64_t time_total; // measures the total time consumed by thread.

  char* exec_path; // path to the binary that is beign executed.
  void* start_address; // start address for new threads.
  unsigned pic_line; // one based PIC line number; for PIC interrupt handlers.
  
  struct thread *this; // logical address of this structure in kernel memory.
  uintptr_t page_table; // physical address to the thread page table.
  
  void* stack_bottom; // range that contains stack; top of the address space.
  void* stack_top;

  domain_t *home_domain;   // TODO: these two are just the first and the last 
  domain_t *current_domain;// elements of domain stack.

  struct domain_stack {
    domain_t *domain;
    uintptr_t return_address;
  } domain_stack[IDC_CALL_MAX];
  size_t domain_idx;
  
} thread_t;

// read-only information about the thread, available for the usermode code.
typedef struct thread_info {
  pid_t thread_id; // unique identifier of this thread.
  pid_t process_id; // process this thread belongs to.
  pid_t parent_id; // process that created this process.
  uid_t uid; // real user id.
  gid_t gid; // real group id.
  uid_t euid; // effective user id.
  gid_t egid; // effective group id.
} thread_info_t;

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

#define CURRENT_THREAD ((thread_t*)_thread_kdata)

#define THREAD_INFO ((thread_info_t*)_thread_uinfo)

void thread_initialize();
thread_t* thread_create(domain_t *home_domain, void *start_address);
void thread_set_current(thread_t* thread);
void thread_calc_time();

void* thread_grow_stack(thread_t *thread, size_t size); // returns new bottom.

#endif
