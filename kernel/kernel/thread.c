#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <kernel/kernel.h>
#include <kernel/domain.h>
#include <kernel/thread.h>
#include <kernel/mem.h>
#include <kernel/vmem.h>
#include <kernel/mmap.h>

// thread data structure is mapped to thread_id*PAGE_SIZE.
// we start from id 1 to keep the zero page empty (to catch NULL pointers).
static uint32_t next_thread_id = 1; 

void thread_initialize() {
  void *ptr = vmem_alloc_existing_table((void*)THREAD_OFFSET);
  uintptr_t new_table = mem_alloc_page();
  mmap_map_table(ptr, new_table, MMAP_ATTRIB_USER_RW);
}

thread_t* thread_create(domain_t *home_domain, void *start_address) {
  if (next_thread_id >= 1023) {
    printf("Out of threads.\n");
    abort();
  }
  uint32_t thread_id = next_thread_id++;

  // allocate required pages.
  uintptr_t page_table = mem_alloc_page();
  uintptr_t thread_page = mem_alloc_page();
  uintptr_t stack_page = mem_alloc_page();
  // we have a virtual memory slot for this thread at thread_id*PAGE_SIZE.
  // first we use that slot to clear the page table:
  void* tmp = (thread_t*)mmap_map_page((void*)(thread_id * PAGE_SIZE),
                                       page_table, MMAP_ATTRIB_KERNEL_RW);
  memset(tmp, 0, PAGE_SIZE);
  // we don't want to put the page table into the page directory yet,
  // so we just manually map the two pages into it. no biggie..
  uint32_t* table_entries = (uint32_t*)tmp;
  table_entries[0] = thread_page | MMAP_ATTRIB_KERNEL_RW;
  table_entries[1] = stack_page | MMAP_ATTRIB_KERNEL_RW;
  table_entries[2] = thread_page | MMAP_ATTRIB_USER_RO;

  // then we can map the actual thread page into memory slot.
  // this way kernel can access the thread structure of in-active threads.
  thread_t* thread =
    (thread_t*)mmap_map_page((void*)(thread_id * PAGE_SIZE),
                             thread_page, MMAP_ATTRIB_KERNEL_RW);
  memset(thread, 0, PAGE_SIZE);

  // initialize the thread structure.
  thread->thread_id = thread_id;
  thread->state = start_address ? THREAD_STATE_NEW : THREAD_STATE_DEAD;
  thread->start_address = start_address;
  thread->this = thread;
  thread->page_table = page_table;
  thread->stack_bottom = thread->stack_top = (void*)(THREAD_OFFSET+TABLE_SIZE);
  // stack will be allocated when the thread becomes active the first time.

  thread->home_domain = home_domain;
  thread->heap_bottom = home_domain->heap_bottom;
  thread->heap_limit = home_domain->heap_limit;
  return thread;
}

void thread_set_current(thread_t* thread) {
  mmap_map_table((void*)THREAD_OFFSET, thread->page_table, MMAP_ATTRIB_USER_RW);
  kernel.current_thread = thread;

  // we post-poned stack allocation until it was actually needed.
  if (thread->stack_bottom == thread->stack_top) {
    thread_grow_stack(thread, 4*PAGE_SIZE);
  }

  domain_enable(thread->home_domain);
}

void* thread_grow_stack(thread_t *thread, size_t size) {
  if (size % PAGE_SIZE) {
    printf("Can grow only full pages.\n");
    abort();
  }
  // thread stack has 1MB limit.
  if (thread->stack_bottom > (void*)THREAD_HEAP_TOP) {
    thread->stack_bottom -= size;
    mem_alloc_mapped(thread->stack_bottom, size, MMAP_ATTRIB_USER_RW);
  } else {
    printf("Thread out of virtual address space!\n");
    abort();
  }
  return thread->stack_bottom;
}
