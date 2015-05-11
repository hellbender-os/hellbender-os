#ifndef _KERNEL_VMEM_H
#define _KERNEL_VMEM_H

#include <stdint.h>

void vmem_stage_3_init();

// allocates TABLE_SIZE of unmapped virtual memory.
void* vmem_alloc_bottom_table();
void* vmem_alloc_top_table();
void* vmem_alloc_existing_table(void* existing);

#endif
