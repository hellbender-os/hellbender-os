#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <kernel/domain.h>
#include <kernel/module.h>
#include <kernel/process.h>
#include <kernel/mem.h>
#include <kernel/mmap.h>
#include <kernel/vmem.h>

void* domain_grow_heap(domain_t *domain, size_t size) {
  if (size % PAGE_SIZE) {
    printf("Can grow only full pages.\n");
    abort();
  }
  if (domain->heap_top + PAGE_SIZE < domain->heap_limit) {
    mem_alloc_mapped(domain->heap_top, size, MMAP_ATTRIB_USER_RW);
    domain->heap_top += size;
  } else {
    printf("Domain out of virtual address space!\n");
    abort();
  }
  return domain->heap_top;
}

domain_t* domain_create() {
  domain_t *domain = malloc(sizeof(domain_t));
  memset(domain, 0, sizeof(domain_t));
  return domain;
}

domain_t* domain_prepare(domain_t *domain, void* base, size_t size) {
  // kernel is special case, page tables are always mapped.
  if (base == KERNEL_BASE) {
    domain->page_table_ds = 0;
    domain->page_table_cs = 0;
  } else {
    domain->page_table_ds = mem_alloc_page();
    domain->page_table_cs = mem_alloc_page();
  }
  domain->domain_base = base;
  domain->domain_size = size;
  domain->module_bottom = domain->module_top = base;
  domain->text_bottom = domain->text_top = base;
  domain->heap_bottom = domain->heap_top = base;
  domain->heap_limit = base + size;
  return domain;
}

/*
domain_t* domain_create_application() {
  domain_t *domain = domain_create();
  return domain_prepare(domain, (void*)APPLICATION_OFFSET, TABLE_SIZE);
  //void *base = vmem_alloc_bottom_table();
  //return domain_prepare(domain, base, TABLE_SIZE);
}

domain_t* domain_create_service() {
  domain_t *domain = domain_create();
  void *base = vmem_alloc_top_table();
  return domain_prepare(domain, base, TABLE_SIZE);
}
*/

domain_t* domain_create_kernel(kernel_module_t* module) {
  domain_t *domain = domain_create();
  domain_prepare(domain, (void*)KERNEL_BASE, TABLE_SIZE);
  
  domain->module_bottom = (void*)KERNEL_BASE;
  domain->module_top = (void*)module->top;
  domain->text_bottom = (void*)module->text_bottom;
  domain->text_top = (void*)module->text_top;
  domain->heap_bottom = domain->heap_top = ceil_page(domain->module_top);
  domain->start = (void*)module->start;
  return domain;
}

domain_t* domain_create_module(kernel_module_t* module,
                               module_binary_t* binary) {
  if (!module_check_header(module)) {
    printf("invalid kernel module!\n");
    abort();
  }

  domain_t *domain = domain_create();
  void* base = vmem_alloc_existing_table((void*)module->bottom);
  domain_prepare(domain, base, TABLE_SIZE);
  if ((uintptr_t)base != module->bottom) {
    printf("Module not loaded at expected address.\n");
    abort();
  }

  // map module to its page table (we have to activate page table for this).
  domain_enable(domain);

  // all data is read/write in DS, all core is read-only in CS & DS.
  unsigned module_size = module->top - module->bottom;
  unsigned binary_size = binary->top - binary->bottom;
  if (module_size % PAGE_SIZE)
    module_size += PAGE_SIZE - module_size % PAGE_SIZE;
  if (binary_size % PAGE_SIZE)
    binary_size += PAGE_SIZE - binary_size % PAGE_SIZE;
  //printf("BINARY R/W: %x-%x into %x-%x\n",
  //       (unsigned)binary->bottom,
  //       (unsigned)binary->bottom + binary_size,
  //       (unsigned)module->bottom,
  //       (unsigned)module->bottom + binary_size);
  mmap_map((void*)module->bottom, binary->bottom,
           module_size, MMAP_ATTRIB_USER_RW);
  unsigned alloc_size = module_size - binary_size;
  if (alloc_size) {
    //printf("MEMORY R/W: into %x-%x\n",
    //       (unsigned)module->bottom + binary_size,
    //       (unsigned)module->bottom + binary_size + alloc_size);
    mem_alloc_mapped((void*)(module->bottom + binary_size), alloc_size,
                     MMAP_ATTRIB_USER_RW);
  }
  unsigned text_offset =
    module->text_bottom - module->bottom;
  unsigned text_size =
    module->text_top - module->text_bottom;
  if (text_size % PAGE_SIZE) text_size += PAGE_SIZE - text_size % PAGE_SIZE;
  //printf("MODULE R/-: %x-%x into %x-%x\n",
  //       (unsigned)binary->bottom + text_offset,
  //       (unsigned)binary->bottom + text_offset + text_size,
  //       (unsigned)module->text_bottom,
  //       (unsigned)module->bottom + text_size);
  mmap_map((void*)(CS_BASE + module->text_bottom),
           binary->bottom + text_offset,
           text_size, MMAP_ATTRIB_USER_RO);
  mmap_map((void*)module->text_bottom,
           binary->bottom + text_offset,
           text_size, MMAP_ATTRIB_USER_RO);
  
  // no need for the page table until some thread accesses it.
  domain_disable(domain);
  
  domain->module_bottom = base;
  domain->module_top = (void*)module->top;
  domain->text_bottom = (void*)module->text_bottom;
  domain->text_top = (void*)module->text_top;
  domain->heap_bottom = domain->heap_top = ceil_page(domain->module_top);
  domain->start = (void*)module->start;
  return domain;
}

void domain_enable(domain_t* domain) {
  kernel.current_domain = domain;
  if (domain->page_table_ds) {
    mmap_map_table(domain->domain_base, domain->page_table_ds,
                   MMAP_ATTRIB_USER_RW);
    mmap_map_table(CS_BASE + domain->domain_base, domain->page_table_cs,
                   MMAP_ATTRIB_USER_RO);
  }
}

void domain_disable(domain_t* domain) {
  if (domain->page_table_ds) {
    mmap_map_table(domain->domain_base, domain->page_table_ds,
                   MMAP_ATTRIB_KERNEL_RW);
    mmap_map_table(CS_BASE + domain->domain_base, domain->page_table_cs,
                   MMAP_ATTRIB_KERNEL_RO);
  }
}

void domain_push(uintptr_t entry_address, uintptr_t return_address) {
  if (CURRENT_THREAD->domain_idx >= IDC_CALL_MAX) {
    printf("Too many recursive IDC calls!\n");
    abort();
  }
  struct domain_stack *stack =
    &CURRENT_THREAD->domain_stack[CURRENT_THREAD->domain_idx++];
  stack->entry_address = entry_address;
  stack->return_address = return_address;
  
  if (entry_address >= APPLICATION_LIMIT
      && entry_address < SERVICE_LIMIT
      && (entry_address & 0x3FF007) == 0) {

    // TODO: map (address>>22) to domain_t.
    domain_enable(kernel.processes[kernel.core_module]->domain);
  } else {
    printf("Illegal IDC address %x.\n", (unsigned)entry_address);
    abort();
  }
}

uintptr_t domain_pop() {
  if (CURRENT_THREAD->domain_idx == 0) {
    printf("Too many IDC pops!\n");
    abort();
  }
  struct domain_stack *stack =
    &CURRENT_THREAD->domain_stack[--CURRENT_THREAD->domain_idx];

  // TODO: disable domain if not in stack anymore.
  return stack->return_address;  
}


/*
void* domain_alloc_data(domain_t *domain, size_t size) {
  if (size % PAGE_SIZE) size += PAGE_SIZE - size % PAGE_SIZE;
  // TODO: support non-multiple-of-page-size.
  void* virtual = domain->data_top;
  domain->data_top = mem_alloc_mapped(virtual, size);
  return virtual;
}

void* domain_alloc_code(domain_t *domain, size_t size) {
  if (size % PAGE_SIZE) size += PAGE_SIZE - size % PAGE_SIZE;
  // TODO: support non-multiple-of-page-size.
  void* virtual = domain->code_top;
  domain->code_top = mem_alloc_mapped(CS_BASE + virtual, size);
  return virtual;
}
*/
