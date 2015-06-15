#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <hellbender/threadlocal.h>

#include <kernel/kernel.h>
#include <kernel/domain.h>
#include <kernel/thread.h>
#include <kernel/mem.h>
#include <kernel/vmem.h>
#include <kernel/mmap.h>

// thread data structure is mapped to thread_id*PAGE_SIZE.
// we start from id 1 to keep the zero page empty (to catch NULL pointers).
static pid_t next_thread_id = 1; 

static uint32_t page_idx(void *addr) {
  return (uint32_t)(((uintptr_t)addr - (uintptr_t)_thread_base) / 0x1000);
}

void thread_initialize() {
  void *ptr = vmem_alloc_existing_table(_thread_base);
  uintptr_t new_table = mem_alloc_page();
  mmap_map_table(ptr, new_table, MMAP_ATTRIB_USER_RW);
}

thread_t* thread_create(domain_t *home_domain, void *start_address) {
  if (next_thread_id >= 1023) {
    printf("Out of threads.\n");
    abort();
  }
  pid_t thread_id = next_thread_id++;

  // allocate required pages.
  uintptr_t page_table = mem_alloc_page();
  uintptr_t thread_page = mem_alloc_page();
  uintptr_t stack_page = mem_alloc_page();
  uintptr_t info_page = mem_alloc_page();
  uintptr_t local_page = mem_alloc_page();

  // we don't want to put the page table into the page directory yet,
  // so we just manually map the two pages into it. no biggie..
  uint32_t* table_entries =
    (uint32_t*)mmap_temp_map(page_table, MMAP_ATTRIB_KERNEL_RW);
  memset(table_entries, 0, PAGE_SIZE);
  table_entries[page_idx(_thread_kdata)] = thread_page | MMAP_ATTRIB_KERNEL_RW;
  table_entries[page_idx(_thread_kstack)] = stack_page | MMAP_ATTRIB_KERNEL_RW;
  table_entries[page_idx(_thread_uinfo)] = info_page | MMAP_ATTRIB_USER_RO;
  table_entries[page_idx(_thread_udata)] = local_page | MMAP_ATTRIB_USER_RW;
  mmap_temp_unmap(table_entries);

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
  thread->stack_bottom = thread->stack_top = _thread_stack_top;
  // stack will be allocated when the thread becomes active the first time.

  thread->home_domain = home_domain;
  thread->domain_stack[0] = (struct domain_stack){ home_domain, 0 };
  thread->domain_idx++;

  // initialize the public thread info structure.
  thread_info_t* info =
    (thread_info_t*)mmap_temp_map(info_page, MMAP_ATTRIB_KERNEL_RW);
  if (kernel.current_thread) {
    *info = (thread_info_t) {
      .thread_id = thread_id,
      .process_id = home_domain->process_id,
      .parent_id = THREAD_INFO->process_id,
      .uid = THREAD_INFO->uid,
      .gid = THREAD_INFO->gid,
      .euid = THREAD_INFO->euid,
      .egid = THREAD_INFO->egid,
    };
  } else { // this is the first thread to be created.
    *info = (thread_info_t) {
      .thread_id = thread_id,
      .process_id = home_domain->process_id,
      .parent_id = thread_id,
      .uid = 0,
      .gid = 0,
      .euid = 0,
      .egid = 0,
    };
  }
  mmap_temp_unmap(info);
  
  return thread;
}

void thread_set_current(thread_t* thread) {
  // Disable all domain in current idc-stack.
  for (size_t i = 0; i < kernel.current_thread->domain_idx; ++i) {
    domain_disable(kernel.current_thread->domain_stack[i].domain);
  }

  // Switch the thread page table.
  mmap_map_table(_thread_base, thread->page_table, MMAP_ATTRIB_USER_RW);
  kernel.current_thread = thread;
  THREADLOCAL->thread_id = thread->thread_id;

  // we post-poned stack allocation until it was actually needed.
  if (thread->stack_bottom == thread->stack_top) {
    thread_grow_stack(thread, 4*PAGE_SIZE);
  }
  
  // Enable all domains in idc-stack
  for (size_t i = 0; i < thread->domain_idx; ++i) {
    domain_enable(thread->domain_stack[i].domain);
  }
}

void thread_calc_time() {
  if (kernel.up_and_running) {
    uint64_t time_end = rdtsc();
    uint64_t time_total = time_end - CURRENT_THREAD->time_start;
    CURRENT_THREAD->time_total += time_total;
  }
}

void* thread_grow_stack(thread_t *thread, size_t size) {
  if (size % PAGE_SIZE) {
    printf("Can grow only full pages.\n");
    abort();
  }
  // thread stack has 1MB limit.
  if (thread->stack_bottom > _thread_stack) {
    thread->stack_bottom -= size;
    mem_alloc_mapped(thread->stack_bottom, size, MMAP_ATTRIB_USER_RW);
  } else {
    printf("Thread out of virtual address space!\n");
    abort();
  }
  return thread->stack_bottom;
}
