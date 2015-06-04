#include <hellbender/heap.h>

#include <kernel/kernel.h>
#include <kernel/mem.h>

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
      // reserve new block. TODO: there is no way to release these ATM!
      size_t alloc_size = HEAP_DEFAULT_ALLOCATION_SIZE;
      uint8_t* bottom = (uint8_t*)heap_malloc_large(heap->large, alloc_size);
      heap->more = (tinyblock_t*)(bottom + 7);
      heap->more_size = alloc_size / sizeof(tinyblock_t) - 1;
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
    // the block we used last is just the right size.
    found = heap->last_block;
    heap->last_idx = 0; // last block no longer available.
    mark_block_used(found);
    return found;

  } else if (heap->last_idx > idx + 1) {
    // last block is bigger than needed; use it anyway for better locality.
    found = heap->last_block;
    set_block_tag(found, idx | BLOCK_RESERVED);
    heap->last_idx -= idx; // split the remaining of the block.
    heap->last_block += idx;
    set_block_tag(heap->last_block, heap->last_idx);
    return found;

  } else { 
    // we are going to overwrite the last block, so push it to free list.
    push_last_block_into_free(heap);

    //  we try to find the next smallest block
    for (unsigned jdx = idx + 2; jdx < 131; ++jdx) {
      if ((found = heap->free[jdx])) {
        unsigned last_idx = get_block_tag(found); // might be bigger than jdx!
        heap->free[jdx] = found->next; // remove the found block.
        if (found->next) found->next->prev = NULL;
        set_block_tag(found, idx | BLOCK_RESERVED);
        heap->last_block = found + idx; // split the remaining.
        heap->last_idx = last_idx - idx;
        set_block_tag(heap->last_block, heap->last_idx);
        return found;
      }
    }

    // no block found in free lists, allocate some room.
    size_t alloc_size = HEAP_DEFAULT_ALLOCATION_SIZE;
    uint8_t *bottom = (uint8_t*)heap_malloc_large(heap->large, alloc_size);
    // we skip one block because the tag is at offset -4.
    found = ((block_t*)bottom) + 1;
    // this helps preceding_block at the boundary.
    *((uint32_t*)bottom) = 0;
    set_block_tag(found, idx | BLOCK_RESERVED);
    heap->last_block = found + idx;
    unsigned new_idx = alloc_size / 8 - 1;
    if (new_idx < idx) {
      printf("Too small increment\n");
      abort();
    }
    heap->last_idx = new_idx - idx; // split the remaining.
    set_block_tag(heap->last_block, heap->last_idx);
    heap->end_block = following_block(heap, heap->last_block);
    return found;
  }
}

void* heap_malloc_large(largeheap_t *heap, size_t size) {
  size = (size + 7) & ~7; // ceil to the next 8 aligned block size.
  unsigned idx = size / 8 + 1; // block size / 8.
  block_t* found = heap->free; // try to find first block that is large enough.
  for (; found; found = found->next) {
    if (get_block_tag(found) >= idx) {
      mark_block_used(found);
      return found;
    }
  }
  // allocate new block of memory large enough for the size.
  // TODO: this whole large heap + wilderness is crap. redesign & rewrite.
  found = ((block_t*)(heap->wilderness->bottom)) + 1;
  uint8_t* end = (uint8_t*)found + size + 8;
  size_t truesize = 0;
  while (heap->wilderness->bottom < end) {
    if (heap->wilderness->bottom == heap->wilderness->top) {
      heap_grow_wilderness(heap->wilderness);
    }
    heap->wilderness->bottom += PAGE_SIZE;
    truesize += PAGE_SIZE;
  }
  unsigned tag = truesize / 8 - 1;
  set_block_tag(found, tag | BLOCK_RESERVED);
  return found;
}
