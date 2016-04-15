#ifndef __HELLBENDER_KERNEL_LOMEM_H__
#define __HELLBENDER_KERNEL_LOMEM_H__

#include "kernel.h"

/* Low memory (<= HIGH_MEMORY_LIMIT) is assumed to be identity mapped
 * by boot_32 code. Thus it is safe to initialize the low memory
 * manager after entering boot_64. Low memory is good for PCI DMA stuff.
 *
 * Low memory is split into page tables (2MB worth of memory).
 * Each table can be either free, allocated, or split.
 * Free table means that no memory within the table is in use.
 * Allocated table means that the whole 2MB table is in use.
 * Split table means that only some of the 4k pages in the table
 * have been allocated.
 * 
 * Memory can be allocated either 2MB table at a time,
 * 4k page at a time, or some multiple of 4k pages at a time.
 * 4k allocations are preferred, since anything more will
 * require a free table, even when allocating less than 2MB.
 */

void lomem_init(struct MultibootData* data);

uintptr_t lomem_alloc_4k();
uintptr_t lomem_alloc_2M();
uintptr_t lomem_alloc_pages(unsigned count);

void lomem_free_4k(uintptr_t page);
void lomem_free_2M(uintptr_t page);
void lomem_free_pages(uintptr_t page, unsigned count);

#endif
