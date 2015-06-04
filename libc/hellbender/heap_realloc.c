#include <hellbender/heap.h>

#include <kernel/kernel.h>
#include <kernel/mem.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "heap_impl.h"

void* heap_realloc_tiny(tinyheap_t* heap, void* ptr, size_t size) {
  (void)(heap);
  if (size <= TINYHEAP_ALLOC_LIMIT) {
    return ptr;
  } else {
    void *newptr = malloc(size);
    memcpy(newptr, ptr, TINYHEAP_ALLOC_LIMIT);
    heap_free_tiny(heap, ptr);
    return newptr;
  }
}

void* heap_realloc_small(smallheap_t* heap, void* ptr, size_t size) {
  block_t *block = (block_t*)ptr;
  unsigned jdx = get_block_tag(block);
  
#ifndef HEAP_NO_CHECK
  // check that ptr seems like a valid heap block:
  if (!is_valid_block(block, jdx)) {
    printf("heap corruption.\n");
    abort();
  }
#endif

  size_t old_size = 8 * jdx - 8;
  if (size <= old_size) return ptr; // TODO: split if new size much smaller.

  size = (size + 7) & ~7; // ceil to the next 8 aligned block size.
  unsigned idx = size / 8 + 1; // block size / 8.

  // first check if the following block has enough room:
  block_t *following = following_block(heap, block);
  if (!is_block_used(following)) {
    unsigned kdx = get_block_tag(following);

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
      set_block_tag(block, idx | BLOCK_RESERVED);
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
      set_block_tag(block, idx | BLOCK_RESERVED);
      heap->last_idx = jdx + kdx - idx; // split the remaining of the block.
      heap->last_block = following + idx - jdx;
      return ptr;
    }
  }
  
  // data needs to be moved (size > old_size).
  void *new_ptr = malloc(size);
  memcpy(new_ptr, ptr, old_size);
  heap_free_small(heap, ptr);
  return new_ptr;
}

void* heap_realloc_large(largeheap_t* heap, void* ptr, size_t size) {
  block_t *block = (block_t*)ptr;
  size_t bsize = get_data_size(block);
  if (size <= bsize) return ptr;
  void *newptr = heap_malloc_large(heap, size);
  memcpy(newptr, ptr, bsize);
  heap_free_large(heap, ptr);
  return newptr;
}
