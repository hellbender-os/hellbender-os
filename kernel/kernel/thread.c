#include <stdint.h>
#include <string.h>

#include <kernel/kernel.h>
#include <kernel/kstdio.h>
#include <kernel/kstdlib.h>
#include <kernel/domain.h>
#include <kernel/thread.h>
#include <kernel/mem.h>

uint32_t next_thread_id = 0;

thread_t* thread_allocate(void *start_address) {
  if (next_thread_id >= 16) {
    kprintf("Out of threads.\n");
    kabort();
  }
  uint32_t thread_id = next_thread_id++;

  // allocate required pages.
  uintptr_t page_table = mem_alloc_page();
  uintptr_t thread_page = mem_alloc_page();
  uintptr_t stack_page = mem_alloc_page();
  
  // we have a virtual memory slot for this thread at thread_id*PAGE_SIZE.
  // first we use that slot to clear the page table:
  void* tmp = (thread_t*)mem_map_page((void*)(thread_id * PAGE_SIZE),
                                      page_table, MEM_ATTRIB_KERNEL);
  memset(tmp, 0, PAGE_SIZE);
  // we don't want to put the page table into the page directory yet,
  // so we just manually map the two pages into it. no biggie..
  uint32_t* table_entries = (uint32_t*)tmp;
  table_entries[0] = thread_page | MEM_ATTRIB_KERNEL;
  table_entries[1] = stack_page | MEM_ATTRIB_KERNEL;
  //mem_map_table((void*)THREAD_OFFSET, page_table, MEM_ATTRIB_USER);

  // then we can map the actual thread page into memory slot.
  // this way kernel can access the thread structure of in-active threads.
  //mem_map_page((void*)THREAD_OFFSET, thread_page, MEM_ATTRIB_KERNEL);
  thread_t* thread = (thread_t*)mem_map_page((void*)(thread_id * PAGE_SIZE),
                                             thread_page, MEM_ATTRIB_KERNEL);
  memset(thread, 0, PAGE_SIZE);

  // allocate kernel stack page, map into the page table.
  //void* kernel_stack = mem_map_page((void*)(THREAD_OFFSET + PAGE_SIZE),
  //                                  stack_page, MEM_ATTRIB_KERNEL);

  // initialize the thread structure.
  //thread->kernel_stack_ptr = (kernel_stack + PAGE_SIZE);
  thread->thread_id = thread_id;
  thread->state = start_address ? THREAD_STATE_NEW : THREAD_STATE_DEAD;
  thread->start_address = start_address;
  thread->this = thread;
  thread->page_table = page_table;
  thread->stack_bottom = thread->stack_top = (void*)(THREAD_OFFSET+TABLE_SIZE);
  // stack will be allocated when the thread becomes active the first time.
  return thread;
}

void thread_set_current(thread_t* thread) {
  mem_map_table((void*)THREAD_OFFSET, thread->page_table, MEM_ATTRIB_USER);

  // we post-poned stack allocation until it was actually needed.
  if (thread->stack_bottom == thread->stack_top) {
    thread_grow_stack(thread, 4*PAGE_SIZE);
  }
}

void* thread_grow_stack(thread_t *thread, size_t size) {
  if (size % PAGE_SIZE) {
    kprintf("Can grow only full pages.\n");
    kabort();
  }
  // thread stack has 1MB limit.
  if (thread->stack_bottom > (void*)(THREAD_OFFSET+TABLE_SIZE-256*PAGE_SIZE)) {
    thread->stack_bottom -= size;
    mem_alloc_mapped(thread->stack_bottom, size);
  } else {
    kprintf("Thread out of virtual address space!\n");
    kabort();
  }
  return thread->stack_bottom;
}
