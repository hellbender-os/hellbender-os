#ifndef _KERNEL_DOMAIN_H
#define _KERNEL_DOMAIN_H

#include <stddef.h>

#include <kernel/kernel.h>

typedef struct domain {
  void* table_base; // address of the page table.
  size_t table_size; // virtual address space size; multiples of TABLE_SIZE.

  void* module_bottom; // address range where domain image was mapped.
  void* module_top;
  void* text_bottom; // range that contains all code; within module_*.
  void* text_top;
  void* heap_bottom; // range that contains heap; above module_top.
  void* heap_top;

  void* start; // address of the entry function.
} domain_t;

// _bottom for applications, _top for services, _existing for core.
domain_t* domain_allocate_bottom();
domain_t* domain_allocate_top();
domain_t* domain_allocate_module(void* module_table);

void* domain_grow_heap(domain_t *domain, size_t size); // returns new top.

#endif
