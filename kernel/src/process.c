#include "process.h"
#include "thread.h"
#include "heap.h"
#include "lomem.h"
#include "page.h"
#include "scheduler.h"

#include <hellbender/libc_init.h>
#include <string.h>

static uint64_t process_next_id = 0;

static uint8_t zero_page[PAGE_SIZE] __attribute__((aligned(PAGE_SIZE)));

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
  p->vmem_base = desc->vmem_base;
  p->vmem_size = desc->vmem_size;
  if (p->vmem_base == USERMODE_OFFSET) {
    p->usermode = 1;
    p->pdpt = (uint64_t*)kernel_p2v(page_clear(lomem_alloc_4k()));
  } else {
    p->pdpt = (uint64_t*)kernel_p2v(page_get_pdpt((void*)p->vmem_base) 
                                    & PAGE_PHYSICAL_MASK);
  }
  p->scheduler.priority = SCHEDULER_PRIORITY_NORMAL;

  // TODO: respect virtual memory bounds.
  // map all memory.
  for (unsigned i = 0; i < desc->n_maps; ++i) {
    struct process_memory *m = desc->memory_maps + i;
    uintptr_t v_base = m->v_base;
    uintptr_t v_size = m->v_size;
    uintptr_t m_base = (uintptr_t)m->m_base;
    intptr_t m_size = m->m_size;
    if (v_base % TABLE_SIZE) kernel_panic();
    if (m_base % PAGE_SIZE) kernel_panic();
    if (v_size % PAGE_SIZE) v_size += PAGE_SIZE - v_size % PAGE_SIZE;
    
    // reserve virtual memory to hold the physical pages.
    struct process_vmem *vmem = process_reserve_vmem(p, (void*)v_base, v_size);
    vmem->flags = m->flags;

    if (m_size > 0) { // map actual memory, starting from bottom.
      if (m_size % PAGE_SIZE) {
        // round physical memory to page boundary, clearing the end of the page.
        intptr_t delta = PAGE_SIZE - m_size % PAGE_SIZE;
        memset((void*)(m_base + m_size), 0, delta);
        m_size += delta;
      }
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

    // fill the rest of the memory with zero page.
    if (v_size && m->flags & PAGE_WRITEABLE) {
      vmem->flags |= PROCESS_VMEM_SEMI_COW;
    }
    while (v_size > 0) {
      process_page_map(p, (void*)v_base, kernel_v2p(zero_page), 
                       (m->flags & ~PAGE_WRITEABLE) | PAGE_USERMODE | PAGE_PRESENT);
      v_base += PAGE_SIZE;
      v_size -= PAGE_SIZE;
    }
  }

  // create thread
  struct thread *t = thread_create(p,
                                   desc->entry_point, desc->stack_top - desc->stack_reserved,
                                   desc->libc);
  list_insert(&p->threads, &t->process_threads);
  scheduler_wakeup(t);
  return p;
}

uint64_t* process_page_table(struct process* proc, uintptr_t address) {
  if (address < proc->vmem_base) kernel_panic();
  if (address + PAGE_SIZE > proc->vmem_base + proc->vmem_size) kernel_panic();
  address -= proc->vmem_base;

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
  return pt;
}

void process_page_map(struct process* proc, void* virt, uintptr_t phys, uint64_t attrib) {
  uintptr_t address = (uintptr_t)virt;
  uint64_t* pt = process_page_table(proc, address);
  uint64_t page_offset = (address>>12) & 0x1FF;
  uint64_t *p_page = pt + page_offset;
  *p_page = (phys & PAGE_PHYSICAL_MASK) | attrib;
}

struct process_vmem* process_reserve_vmem(struct process* proc, void* base, size_t size) {
  // each allocated vmem area has a descriptor structure.
  struct process_vmem *vmem = HEAP_NEW(struct process_vmem);
  vmem->base = base;
  vmem->size = size;
  list_insert(&proc->vmem, &vmem->item);

  // the first page table is the vmem descriptor pointer table.
  uint64_t* pt = process_page_table(proc, proc->vmem_base);
  // each page in the VDPT described 512 vmem blocks, while each block is 2MB.
  uintptr_t address = (uintptr_t)base;
  uint64_t page_offset = address / TABLEDIR_SIZE;
  uint64_t *p_page = pt + page_offset;
  if (!*p_page) *p_page = page_clear(lomem_alloc_4k()) | PAGE_KERNEL_RW;
  struct process_vmem **vdpt = (struct process_vmem**)kernel_p2v(*p_page & PAGE_PHYSICAL_MASK);
  uint64_t vdp_offset = (address / TABLE_SIZE) % (USERMODE_SIZE / TABLE_SIZE);

  // add the descriptor pointer for every vmem block in this vmem area.
  while (size > 0) {
    vdpt[vdp_offset] = vmem;
    if (size > TABLE_SIZE) {
      address += TABLE_SIZE;
      size -= TABLE_SIZE;
      if (++vdp_offset > 511) { // switch VDPT if we cross over.
        page_offset = address / TABLEDIR_SIZE;
        p_page = pt + page_offset;
        if (!*p_page) *p_page = page_clear(lomem_alloc_4k()) | PAGE_KERNEL_RW;
        vdpt = (struct process_vmem**)kernel_p2v(*p_page & PAGE_PHYSICAL_MASK);
        vdp_offset = 0;
      }
    }
    else break;
  }

  return vmem;
}

struct process_vmem* process_get_vmem(struct process* proc, void* ptr) {
  uint64_t* pt = process_page_table(proc, proc->vmem_base);
  uintptr_t address = (uintptr_t)ptr;
  uint64_t page_offset = address / TABLEDIR_SIZE;
  uint64_t *p_page = pt + page_offset;
  if (!*p_page) return 0;
  struct process_vmem **vdpt = (struct process_vmem**)kernel_p2v(*p_page & PAGE_PHYSICAL_MASK);
  uint64_t vdp_offset = (address / TABLE_SIZE) % (USERMODE_SIZE / TABLE_SIZE);
  return vdpt[vdp_offset];
}
