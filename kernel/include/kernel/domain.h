#ifndef _KERNEL_DOMAIN_H
#define _KERNEL_DOMAIN_H

#include <stddef.h>

#include <kernel/kernel.h>

typedef struct domain {
  void* domain_base; // beginning of the virtual address space.
  size_t domain_size; // virtual address space size; multiples of TABLE_SIZE.
  // TODO: support domains larger than TABLE_SIZE.
  // TODO: support domains with holes in their virtual memory.
  uintptr_t page_table_ds; // physical address of the page table.
  uintptr_t page_table_cs; // physical address of the page table.

  void* module_bottom; // address range where domain image was mapped.
  void* module_top;
  void* text_bottom; // range that contains all code; within module_*.
  void* text_top;
  void* heap_bottom; // range that contains heap; above module_top.
  void* heap_top;
  void* heap_limit;

  void* start; // address of the entry function.
} domain_t;

// application domain from fixed addess,
//domain_t* domain_create_application(); // fixed at APPLICATION_OFFSET
//domain_t* domain_create_service(); // allocated from the top of address space.
domain_t* domain_create_kernel(kernel_module_t* module);
domain_t* domain_create_module(kernel_module_t* module,
                               module_binary_t* binary); // from loaded binary.
domain_t* domain_create_application(); // app bootstrap.

void domain_update_data(domain_t *domain, void* address, size_t size);
void domain_update_text(domain_t *domain, void* address, size_t size);

void* domain_grow_heap(domain_t *domain, size_t size); // returns new top.

void domain_enable(domain_t* domain);
void domain_disable(domain_t* domain);

void domain_push(uintptr_t entry_address, uintptr_t return_address);
uintptr_t domain_pop();

#endif
