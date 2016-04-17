#include "process.h"
#include "heap.h"
#include "lomem.h"
#include "page.h"

#include <string.h>

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
  p->pdpt = (uint64_t*)kernel_p2v(page_clear(lomem_alloc_4k()));

  // map all memory.
  for (unsigned i = 0; i < desc->n_maps; ++i) {
    struct process_memory *m = desc->memory_maps + i;
    uintptr_t v_base = m->v_base;
    uintptr_t v_size = m->v_size;
    uintptr_t m_base = (uintptr_t)m->m_base;
    uintptr_t m_size = m->m_size;
    if (v_base % PAGE_SIZE) kernel_panic();
    if (v_size % PAGE_SIZE) kernel_panic();
    if (m_base % PAGE_SIZE) kernel_panic();
    if (m_size % PAGE_SIZE) kernel_panic();
    
    while (v_size > 0 && m_size > 0) {
      uintptr_t address = page_get_address((void*)m_base);
      process_page_map(p, (void*)v_base, address, m->flags);

      v_base += PAGE_SIZE;
      v_size -= PAGE_SIZE;
      m_base += PAGE_SIZE;
      m_size -= PAGE_SIZE;
    }
  }

  // create thread
  p->threads[0] = create_thread(desc->entry_point, desc->stack_top);
  return 0;
}

void process_page_map(struct process* proc, void* virt, uintptr_t phys, uint64_t attrib) {
  uintptr_t address = (uintptr_t)virt;

  // we support just 512GB of virtual address space/process.
  uint64_t pdpt_offset = (address>>36) & 0xFF8;
  if (pdpt_offset > 0) kernel_panic();
  uint64_t *pdpt = proc->pdpt;

  uint64_t pd_offset = (address>>27) & 0xFF8;
  uint64_t *p_pd = pdpt + pd_offset;
  if (!*p_pd) *p_pd = page_clear(lomem_alloc_4k()) | attrib;
  uint64_t *pd = (uint64_t*)kernel_p2v(*p_pd & PAGE_ADDRESS_MASK);

  uint64_t pt_offset = (address>>18) & 0xFF8;
  uint64_t *p_pt = pd + pt_offset;
  if (!*p_pt) *p_pt = page_clear(lomem_alloc_4k()) | attrib;
  uint64_t* pt = (uint64_t*)kernel_p2v(*p_pt & PAGE_ADDRESS_MASK);
  
  uint64_t page_offset = (address>>9) & 0xFF8;
  uint64_t *p_page = pt + page_offset;
  *p_page = (phys & PAGE_ADDRESS_MASK) | attrib;
}
