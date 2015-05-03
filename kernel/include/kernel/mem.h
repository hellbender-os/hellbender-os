#ifndef _KERNEL_MEM_H
#define _KERNEL_MEM_H

#include <stdint.h>

#include <kernel/multiboot.h>

void mem_early_initialize();
void mem_early_finalize(memory_map_t *memory_map, unsigned map_elements);
void* mem_early_map_page(void* address, uintptr_t page, unsigned attributes);
void* mem_early_map(void* virtual, uintptr_t physical,
                    size_t size, unsigned attributes);
void mem_early_enable_paging();

uintptr_t mem_alloc_page();
void mem_free_page(uintptr_t page);
void* mem_map_page(void* virtual, uintptr_t physical, unsigned attributes);
uintptr_t mem_unmap_page(void* address);
void* mem_map(void* virtual, uintptr_t physical,
              size_t size, unsigned attributes);

#endif
