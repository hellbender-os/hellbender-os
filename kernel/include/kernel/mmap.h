#ifndef _KERNEL_MMAP_H
#define _KERNEL_MMAP_H

#include <stdint.h>

#define MMAP_ATTRIB_USERMODE 4
#define MMAP_ATTRIB_WRITEABLE 2
#define MMAP_ATTRIB_PRESENT 1
#define MMAP_ATTRIB_KERNEL_RW (1|2)
#define MMAP_ATTRIB_KERNEL_RO (1)
#define MMAP_ATTRIB_USER_RW (1|2|4)
#define MMAP_ATTRIB_USER_RO (1|4)

void mmap_stage_2_init();
void* mmap_stage_2_map_page(void* address, uintptr_t page, unsigned attributes);
void* mmap_stage_2_map(void* virtual, uintptr_t physical,
                       size_t size, unsigned attributes);
void mmap_stage_2_enable_paging();
void mmap_stage_2_cleanup();

void* mmap_map_page(void* virtual, uintptr_t physical, unsigned attributes);
uintptr_t mmap_unmap_page(void* address);
uintptr_t mmap_remap_page(void* virtual, unsigned attributes);
void mmap_mirror_page(void* destination, void* source);

void* mmap_map(void* virtual, uintptr_t physical,
               size_t size, unsigned attributes);
void mmap_unmap(void* virtual, size_t size);
void mmap_remap(void* virtual, size_t size, unsigned attributes);
void mmap_mirror(void* destination, void* source, size_t size);

void* mmap_map_table(void* virtual, uintptr_t page_table, unsigned attributes);

void* mmap_temp_map(uintptr_t page, unsigned attributes);
void mmap_temp_unmap(void*);

#endif
