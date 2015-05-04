#include <string.h>

#include <kernel/domain.h>
#include <kernel/mem.h>
#include <kernel/kstdio.h>
#include <kernel/kstdlib.h>

typedef struct domain {
  void* base;
  void* data_top;
  void* code_top;
  
} domain_t;

#define DOMAINS_LIMIT 16
domain_t domains[DOMAINS_LIMIT];
unsigned domains_count = 0;

domain_t* domain_create() {
  if (domains_count < DOMAINS_LIMIT) {
    domain_t *domain = &domains[domains_count++];
    memset(domain, 0, sizeof(domain_t));
    domain->base = mem_alloc_table();
    domain->data_top = domain->code_top = domain->base;
    return domain;
  } else {
    kprintf("Out of domains!\n");
    kabort();
  }
}

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
