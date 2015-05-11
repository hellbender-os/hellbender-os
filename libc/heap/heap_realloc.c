
#include <kernel/kernel.h>
#include <kernel/mem.h>
#include <sys/heap.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "heap_impl.h"

void* heap_realloc_tiny(tinyheap_t* heap, void* ptr, size_t size) {
  (void)(heap); (void)(ptr); (void)(size);
  return NULL;
}

void* heap_realloc_small(smallheap_t* heap, void* ptr, size_t size) {
  if (ptr == NULL) return heap_malloc_small(heap, size);
  if (size == 0) {
    heap_free_small(heap, ptr);
    return NULL;
  }

  block_t *block = (block_t*)ptr;
  unsigned jdx = get_block_size(block);
#ifndef HEAP_NO_CHECK
  // check that ptr seems like a valid heap block:
  if (!is_valid_block(block, jdx)) {
    printf("heap corruption.\n");
    abort();
  }
#endif

  size = (size + 7) & ~7; // ceil to the next 8 aligned block size.
  unsigned idx = size / 8 + 1; // block size / 8.

  // first check if the following block has enough room:
  block_t *following = following_block(heap, block);
  if (!is_block_used(following)) {
    unsigned kdx = get_block_size(following);

    if (jdx + kdx == idx) { // we just have to merge the two blocks.
      // remove following block from the free list.
      if (following->prev) {
        following->prev->next = following->next;
      } else {
        heap->free[kdx] = following->next;
      }
      if (following->next) {
        following->next->prev = following->prev;
      }
      set_block_size(block, idx | BLOCK_RESERVED);
      return ptr;

    } else if (jdx + kdx > idx) { // merge and split the remaining.
      // we are going to overwrite the last block, so push it to free list.
      if (heap->last_idx) {
        heap->last_block->prev = NULL;
        block_t *next;
        if ((next = heap->last_block->next = heap->free[heap->last_idx])) {
          next->prev = heap->last_block;
        }
        heap->free[heap->last_idx] = heap->last_block;
      }
      set_block_size(block, idx | BLOCK_RESERVED);
      heap->last_idx = jdx + kdx - idx; // split the remaining of the block.
      heap->last_block = following + idx - jdx;
      return ptr;
    }
  }
  
  // data needs to be moved.
  void *new_ptr = heap_malloc_small(heap, size);
  size_t old_size = 8*jdx;
  memcpy(new_ptr, ptr, old_size < size ? old_size : size);
  heap_free_small(heap, ptr);
  return new_ptr;
}
