#ifndef _KERNEL_MEM_H
#define _KERNEL_MEM_H

#include <stdint.h>
#include <kernel/multiboot.h>

#define MEM_ATTRIB_USERMODE 4
#define MEM_ATTRIB_WRITEABLE 2
#define MEM_ATTRIB_PRESENT 1
#define MEM_ATTRIB_KERNEL (1|2)
#define MEM_ATTRIB_USER (1|2|4)

void mem_early_initialize();
void mem_early_finalize(memory_map_t *memory_map, unsigned map_elements,
                        uintptr_t kernel_bottom, uintptr_t kernel_top,
                        uintptr_t core_bottom, uintptr_t core_top);
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
void mem_unmap(void* virtual, size_t size);
void* mem_alloc_mapped(void *virtual, size_t size);

void* mem_map_table(void* virtual, uintptr_t page_table, unsigned attributes);

#endif
