#ifndef __HELLBENDER_KERNEL_HEAP_H__
#define __HELLBENDER_KERNEL_HEAP_H__

#include "config.h"

/* Kernel heap allows one to allocate/free 60+N*64 byte blocks.
 * Maximum allocation limit (HEAP_MAX_ALLOC) cannot be exceeded.
 * Returned pointers are always aligned to cache line boundary.
 *
 * Each CPU has its own heap with lists for block sizes N=0..15.
 * If lists are empty, memory is allocated from a shared
 * wilderness area.
 *
 * Wilderness is allocated in 2MB increments from low memory.
 * Wilderness is never released.
 */

#define HEAP_MAX_ALLOC 1020
#define N_FREE_LISTS ((HEAP_MAX_ALLOC)/(CACHE_LINE)+1)

struct heap_block;
typedef struct heap {
  struct heap_block *free_lists[N_FREE_LISTS];
} heap_t;

void* heap_alloc(unsigned bytes);
void heap_free(void* ptr);

#endif
