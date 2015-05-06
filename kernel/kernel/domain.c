#include <string.h>

#include <kernel/domain.h>
#include <kernel/module.h>
#include <kernel/mem.h>
#include <kernel/kstdio.h>
#include <kernel/kstdlib.h>

typedef struct domain {
  void* table_base; // address of the page table.
  size_t table_size; // virtual address space size; multiples of TABLE_SIZE.

  void* module_bottom; // address range where domain image was mapped.
  void* module_top;
  void* text_bottom; // range that contains all code; within module_*.
  void* text_top;
  void* heap_bottom; // range that contains heap; above module_top.
  void* heap_top;
  void* stack_bottom; // range that contains stack; top of the address space.
  void* stack_top;

  void* start; // address of the entry function.
} domain_t;

#define DOMAINS_LIMIT 16
domain_t domains[DOMAINS_LIMIT];
unsigned domains_count = 0;

void* domain_grow_heap(domain_t *domain, size_t size) {
  if (size % PAGE_SIZE) {
    kprintf("Can grow only full pages.\n");
    kabort();
  }
  if (domain->heap_top + PAGE_SIZE < domain->stack_bottom - PAGE_SIZE) {
    mem_alloc_mapped(domain->stack_bottom, size);
    domain->heap_top += size;
  } else {
    kprintf("Domain out of virtual address space!\n");
    kabort();
  }
  return domain->heap_top;
}

void* domain_grow_stack(domain_t *domain, size_t size) {
  if (size % PAGE_SIZE) {
    kprintf("Can grow only full pages.\n");
    kabort();
  }
  if (domain->heap_top + PAGE_SIZE < domain->stack_bottom - PAGE_SIZE) {
    domain->stack_bottom -= size;
    mem_alloc_mapped(domain->stack_bottom, size);
  } else {
    kprintf("Domain out of virtual address space!\n");
    kabort();
  }
  return domain->stack_bottom;
}

domain_t* domain_allocate() {
  if (domains_count < DOMAINS_LIMIT) {
    // find room for the domain structure.
    domain_t *domain = &domains[domains_count++];
    memset(domain, 0, sizeof(domain_t));
    return domain;
  } else {
    kprintf("Out of domains!\n");
    kabort();
  }
}

domain_t* domain_prepare(domain_t *domain, void* base, size_t size) {
  domain->table_base = base;
  domain->table_size = size;
  domain->module_bottom = domain->module_top = base;
  domain->text_bottom = domain->text_top = base;
  domain->heap_bottom = domain->heap_top = base;
  domain->stack_bottom = domain->stack_top = base + size;
  domain_grow_stack(domain, PAGE_SIZE);
  return domain;
}

domain_t* domain_allocate_bottom() {
  domain_t *domain = domain_allocate();
  void *base = mem_alloc_bottom_table();
  return domain_prepare(domain, base, TABLE_SIZE);
}

domain_t* domain_allocate_top() {
  domain_t *domain = domain_allocate();
  void *base = mem_alloc_top_table();
  return domain_prepare(domain, base, TABLE_SIZE);
}

domain_t* domain_allocate_module(void* module_table) {
  domain_t *domain = domain_allocate();
  void* base = mem_alloc_existing_table(module_table);
  domain_prepare(domain, base, TABLE_SIZE);

  // module is already fully mapped in CS and DS.
  kernel_module_t *module = (kernel_module_t*)module_table;
  if (!module_check_header(module)) {
    kprintf("invalid core module!\n");
    kabort();
  }
  void* relative = base - module->bottom;
  domain->module_bottom = base;
  domain->module_top = relative + module->top;
  domain->text_bottom = relative + module->text_bottom;
  domain->text_top = relative + module->text_top;
  domain->heap_bottom = domain->heap_top = ceil_page(domain->module_top);
  domain->start = relative + module->start;

  // protect text segment by unmapping it from DS.
  mem_unmap(domain->text_bottom, domain->text_top - domain->text_bottom);
  return domain;
}

void domain_enter_ring3(domain_t *domain) {
  if (!domain->start) {
    kprintf("Domain doesn't have an entry function.\n");
    kabort();
  }
  kernel_enter_ring3(SEL_USER_DATA|3, (uint32_t)domain->stack_top,
                     SEL_USER_CODE|3, (uint32_t)domain->start);
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
