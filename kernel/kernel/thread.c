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
  // TODO: ASSUMES THERE IS NO ACTIVE THREAD AT THE MOMENT!
  uint32_t thread_id = next_thread_id++;

  // allocate page table, clear it, and put into the page directory.
  uintptr_t page_table = mem_alloc_page();
  void* tmp = (thread_t*)mem_map_page((void*)(thread_id * PAGE_SIZE),
                                      page_table, MEM_ATTRIB_KERNEL);
  memset(tmp, 0, PAGE_SIZE);
  mem_map_table((void*)THREAD_OFFSET, page_table, MEM_ATTRIB_USER);

  // allocate thread page, map into thread and kernel memories.
  uintptr_t thread_page = mem_alloc_page();
  mem_map_page((void*)THREAD_OFFSET, thread_page, MEM_ATTRIB_KERNEL);
  thread_t* thread = (thread_t*)mem_map_page((void*)(thread_id * PAGE_SIZE),
                                             thread_page, MEM_ATTRIB_KERNEL);
  memset(thread, 0, PAGE_SIZE);

  // allocate kernel stack page, map into the page table.
  uintptr_t stack_page = mem_alloc_page();
  void* kernel_stack = mem_map_page((void*)(THREAD_OFFSET + PAGE_SIZE),
                                    stack_page, MEM_ATTRIB_KERNEL);

  // prepare TSS if this was the first thread created
  // (all threads use the same value).
  if (thread_id == 0) {
    kernel_tss.esp0 = (uint32_t)(kernel_stack + PAGE_SIZE);
    tss_update();
  }

  // initialize the thread structure.
  thread->kernel_stack_ptr = (kernel_stack + PAGE_SIZE);
  thread->thread_id = thread_id;
  thread->state = THREAD_STATE_NEW;
  thread->start_address = start_address;
  thread->this = thread;
  thread->page_table = page_table;
  thread->stack_bottom = thread->stack_top = (void*)(THREAD_OFFSET+TABLE_SIZE);
  thread_grow_stack(thread, 4*PAGE_SIZE);

  return thread;
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
