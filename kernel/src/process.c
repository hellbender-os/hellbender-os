#include "process.h"
#include "thread.h"
#include "heap.h"
#include "lomem.h"
#include "page.h"

#include <string.h>

static uint64_t process_next_id = 0;

struct process_descriptor* process_alloc_descriptor(unsigned n_maps) {
  unsigned pd_size = sizeof(struct process_descriptor) 
    + (n_maps - 1) * sizeof(struct process_memory);
  struct process_descriptor* pd = (struct process_descriptor*)heap_alloc(pd_size);
  memset(pd, 0, pd_size);
  return pd;
}

struct process* process_create(struct process_descriptor* desc) {
  // allocate process structure.
  struct process* p = heap_alloc(sizeof(struct process));
  memset(p, 0, sizeof(struct process));
  p->pid = ++process_next_id;
  p->pcid = p->pid & PROCESS_CONTEXT_MASK;
  p->pdpt = (uint64_t*)kernel_p2v(page_clear(lomem_alloc_4k()));

  // map all memory.
  for (unsigned i = 0; i < desc->n_maps; ++i) {
    struct process_memory *m = desc->memory_maps + i;
    uintptr_t v_base = m->v_base;
    uintptr_t v_size = m->v_size;
    uintptr_t m_base = (uintptr_t)m->m_base;
    intptr_t m_size = m->m_size;
    if (v_base % PAGE_SIZE) kernel_panic();
    if (m_base % PAGE_SIZE) kernel_panic();
    if (v_size % PAGE_SIZE) v_size += PAGE_SIZE - v_size % PAGE_SIZE;
    
    if (m_size > 0) { // map actual memory, starting from bottom.
      if (m_size % PAGE_SIZE) m_size += PAGE_SIZE - m_size % PAGE_SIZE;
      while (v_size > 0 && m_size > 0) {
        uintptr_t address = page_get_address((void*)m_base);
        process_page_map(p, (void*)v_base, address, 
                         m->flags | PAGE_USERMODE | PAGE_PRESENT);
        v_base += PAGE_SIZE;
        v_size -= PAGE_SIZE;
        m_base += PAGE_SIZE;
        m_size -= PAGE_SIZE;
      }
    } else { // map actual memory, starting from top.
      m_size = -m_size;
      if (m_size % PAGE_SIZE) m_size += PAGE_SIZE - m_size % PAGE_SIZE;
      while (v_size > 0 && m_size > 0) {
        uintptr_t address = page_get_address((void*)(m_base + m_size - PAGE_SIZE));
        process_page_map(p, (void*)(v_base + v_size - PAGE_SIZE), address, 
                         m->flags | PAGE_USERMODE | PAGE_PRESENT);
        v_size -= PAGE_SIZE;
        m_size -= PAGE_SIZE;
      }
    }

    // allocate more memory. TODO: use on demand allocation.
    while (v_size > 0) {
      uintptr_t address = page_clear(lomem_alloc_4k());
      process_page_map(p, (void*)v_base, address, 
                       m->flags | PAGE_USERMODE | PAGE_PRESENT);
      v_base += PAGE_SIZE;
      v_size -= PAGE_SIZE;
    }
  }

  // create thread
  struct thread *t = thread_create(p,
                                   desc->entry_point, desc->stack_top,
                                   desc->local_bottom, desc->local_size);

  if (p->n_threads == PROCESS_MAX_THREADS) kernel_panic();
  p->threads[p->n_threads++] = t;
  return p;
}

void process_page_map(struct process* proc, void* virt, uintptr_t phys, uint64_t attrib) {
  uintptr_t address = (uintptr_t)virt;

  // we support just 512GB of virtual address space/process.
  uint64_t pdpt_offset = (address>>39) & 0x1FF;
  if (pdpt_offset > 0) kernel_panic();
  uint64_t *pdpt = proc->pdpt;

  uint64_t pd_offset = (address>>30) & 0x1FF;
  uint64_t *p_pd = pdpt + pd_offset;
  if (!*p_pd) *p_pd = page_clear(lomem_alloc_4k()) | PAGE_USER_RW;
  uint64_t *pd = (uint64_t*)kernel_p2v(*p_pd & PAGE_PHYSICAL_MASK);

  uint64_t pt_offset = (address>>21) & 0x1FF;
  uint64_t *p_pt = pd + pt_offset;
  if (!*p_pt) *p_pt = page_clear(lomem_alloc_4k()) | PAGE_USER_RW;
  uint64_t* pt = (uint64_t*)kernel_p2v(*p_pt & PAGE_PHYSICAL_MASK);
  
  uint64_t page_offset = (address>>12) & 0x1FF;
  uint64_t *p_page = pt + page_offset;
  *p_page = (phys & PAGE_PHYSICAL_MASK) | attrib;
}
