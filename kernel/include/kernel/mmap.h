#ifndef _KERNEL_MMAP_H
#define _KERNEL_MMAP_H

#include <stdint.h>

#define MMAP_ATTRIB_USERMODE 4
#define MMAP_ATTRIB_WRITEABLE 2
#define MMAP_ATTRIB_PRESENT 1
#define MMAP_ATTRIB_KERNEL (1|2)
#define MMAP_ATTRIB_USER (1|2|4)

void mmap_early_initialize();
void mmap_early_finalize();
void* mmap_early_map_page(void* address, uintptr_t page, unsigned attributes);
void* mmap_early_map(void* virtual, uintptr_t physical,
                     size_t size, unsigned attributes);
void mmap_early_enable_paging();

void* mmap_map_page(void* virtual, uintptr_t physical, unsigned attributes);
uintptr_t mmap_unmap_page(void* address);
uintptr_t mmap_remap_page(void* virtual, unsigned attributes);
void* mmap_map(void* virtual, uintptr_t physical,
               size_t size, unsigned attributes);
void mmap_unmap(void* virtual, size_t size);
void mmap_remap(void* virtual, size_t size, unsigned attributes);

void* mmap_map_table(void* virtual, uintptr_t page_table, unsigned attributes);

#endif
