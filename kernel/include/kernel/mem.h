#ifndef _KERNEL_MEM_H
#define _KERNEL_MEM_H

#include <stdint.h>
#include <kernel/multiboot.h>

void mem_early_initialize(memory_map_t *memory_map, unsigned map_elements,
                          uintptr_t kernel_bottom, uintptr_t kernel_top,
                          uintptr_t core_bottom, uintptr_t core_top);

uintptr_t mem_alloc_page();
void mem_free_page(uintptr_t page);
void* mem_alloc_mapped(void *virtual, size_t size);

#endif
