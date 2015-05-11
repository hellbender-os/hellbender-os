#ifndef _KERNEL_MEM_H
#define _KERNEL_MEM_H

#include <stdint.h>
#include <kernel/multiboot.h>
#include <kernel/module.h>

void mem_stage_3_init(memory_map_t *memory_map, unsigned map_elements,
                      module_binary_t *binaries, unsigned nof_binaries);

uintptr_t mem_alloc_page();
void mem_free_page(uintptr_t page);
void* mem_alloc_mapped(void *virtual, size_t size, unsigned mmap_attr);

#endif
