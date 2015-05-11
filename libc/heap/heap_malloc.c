
#include <kernel/kernel.h>
#include <kernel/mem.h>
#include <sys/heap.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "heap_impl.h"

void* heap_malloc_tiny(tinyheap_t *heap, size_t size) {
  if (size == 0) return NULL;
  if (size > 7) {
    printf("too large allocation for tiny heap.\n");
    abort();
    return NULL;
  }
  
  tinyblock_t *found;
  if ((found = heap->some)) {
    // re-use freed block.
    heap->some = found->free.next;
    return found->used.data;

  } else {
    if (heap->more_size == 0) {
      // reserve new block.
      if (heap->wilderness->bottom == heap->wilderness->top) {
        heap_grow_wilderness(heap->wilderness);
      }
      heap->more = (tinyblock_t*)(((uint8_t*)heap->wilderness->bottom) + 7);
      heap->more_size = PAGE_SIZE / sizeof(tinyblock_t) - 1;
      heap->wilderness->bottom += PAGE_SIZE;
    }
    
    // take next block from current array.
    found = heap->more++;
    --heap->more_size;
    found->free.tag = 0xff;
    return found->used.data;
  }
}
 
void* heap_malloc_small(smallheap_t *heap, size_t size) {
  if (size == 0) return NULL;
  if (size > 1024) {
    printf("too large allocation size.\n");
    abort();
  }
  size = (size + 7) & ~7; // ceil to the next 8 aligned block size.
  block_t* found = NULL; // try to find a suitable block from the heap.
  unsigned idx = size / 8 + 1; // block size / 8.
  if ((found = heap->free[idx])) {
    // we found just the right sized block, remove it from the free list.
    heap->free[idx] = found->next;
    if (found->next) found->next->prev = NULL;
    mark_block_used(found);
    return found;

  } else if (heap->fred_idx == idx) {
    // the released block is just the right size.
    found = heap->fred_block;
    heap->fred_idx = 0; // the block is no longer available.
    mark_block_used(found);
    return found;

  } else if (heap->last_idx == idx) {
    // the block we use last is jus the right size.
    found = heap->last_block;
    heap->last_idx = 0; // last block no longer available.
    mark_block_used(found);
    return found;

  } else if (heap->last_idx > idx + 1) {
    // last block is bigger than needed; use it anyway for better locality.
    found = heap->last_block;
    set_block_size(found, idx | BLOCK_RESERVED);
    heap->last_idx -= idx; // split the remaining of the block.
    heap->last_block += idx;
    set_block_size(heap->last_block, heap->last_idx);
    return found;

  } else { 
    // we are going to overwrite the last block, so push it to free list.
    push_last_block_into_free(heap);

    //  we try to find the next smallest block
    for (unsigned jdx = idx + 2; jdx < 131; ++jdx) {
      if ((found = heap->free[jdx])) {
        unsigned last_size = get_block_size(found); // might be bigger than jdx!
        heap->free[jdx] = found->next; // remove the found block.
        if (found->next) found->next->prev = NULL;
        set_block_size(found, idx | BLOCK_RESERVED);
        heap->last_block = found + idx; // split the remaining.
        heap->last_idx = last_size - idx;
        set_block_size(heap->last_block, heap->last_idx);
        return found;
      }
    }

    // no block found in free lists, take a page from the wilderness.
    if (heap->wilderness->bottom == heap->wilderness->top) {
      heap_grow_wilderness(heap->wilderness);
    }
    // we skip one because size is at offset -4.
    found = ((block_t*)(heap->wilderness->bottom)) + 1;
    // this help preceding_block at the boundary.
    *((uint32_t*)heap->wilderness->bottom) = 0;
    heap->wilderness->bottom += PAGE_SIZE;
    set_block_size(found, idx | BLOCK_RESERVED);
    heap->last_block = found + idx;
    unsigned new_idx = PAGE_SIZE/8 - 2;
    if (new_idx < idx) {
      printf("Too small increment\n");
      abort();
    }
    heap->last_idx = new_idx - idx; // split the remaining.
    set_block_size(heap->last_block, heap->last_idx);
    heap->end_block = following_block(heap, heap->last_block);
    return found;
  }
  
  printf("too large allocation size %u\n", (unsigned)size);
  abort();
}
