#include <string.h>

#include <kernel/domain.h>
#include <kernel/module.h>
#include <kernel/mem.h>
#include <kernel/mmap.h>
#include <kernel/vmem.h>
#include <kernel/kstdio.h>
#include <kernel/kstdlib.h>

#define DOMAINS_LIMIT 16
domain_t domains[DOMAINS_LIMIT];
unsigned domains_count = 0;

void* domain_grow_heap(domain_t *domain, size_t size) {
  if (size % PAGE_SIZE) {
    kprintf("Can grow only full pages.\n");
    kabort();
  }
  if (domain->heap_top + PAGE_SIZE < domain->table_base + domain->table_size) {
    mem_alloc_mapped(domain->heap_top, size);
    domain->heap_top += size;
  } else {
    kprintf("Domain out of virtual address space!\n");
    kabort();
  }
  return domain->heap_top;
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
  return domain;
}

domain_t* domain_allocate_bottom() {
  domain_t *domain = domain_allocate();
  void *base = vmem_alloc_bottom_table();
  return domain_prepare(domain, base, TABLE_SIZE);
}

domain_t* domain_allocate_top() {
  domain_t *domain = domain_allocate();
  void *base = vmem_alloc_top_table();
  return domain_prepare(domain, base, TABLE_SIZE);
}

domain_t* domain_allocate_module(void* module_table) {
  domain_t *domain = domain_allocate();
  void* base = vmem_alloc_existing_table(module_table);
  domain_prepare(domain, base, TABLE_SIZE);

  // module is already fully mapped in CS and DS.
  kernel_module_t *module = (kernel_module_t*)module_table;
  if (!module_check_header(module)) {
    kprintf("invalid kernel module!\n");
    kabort();
  }
  void* relative = base - module->bottom;
  if (relative) {
    kprintf("Module not loaded at expected address.\n");
    kabort();
  }
  domain->module_bottom = base;
  domain->module_top = relative + module->top;
  domain->text_bottom = relative + module->text_bottom;
  domain->text_top = relative + module->text_top;
  domain->heap_bottom = domain->heap_top = ceil_page(domain->module_top);
  domain->start = relative + module->start;
  return domain;
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
