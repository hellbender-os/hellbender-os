#include <hellbender/heap.h>

#include <kernel/kernel.h>
#include <kernel/mem.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "heap_impl.h"

void heap_free_tiny(tinyheap_t *heap, void* ptr) {
  tinyblock_t* block = (tinyblock_t*)(((uint8_t*)ptr)-1);
  if (block->used.tag != 0xff) {
    printf("not a tinyheap block!\n");
    abort();
  }
  block->free.next = heap->some;
  heap->some = block;
}

void heap_free_small(smallheap_t *heap, void* ptr) {
  block_t *block = (block_t*)ptr;
  unsigned idx = get_block_tag(block);
#ifndef HEAP_NO_CHECK
  // check that ptr seems like a valid heap block:
  if (!is_valid_block(block, idx)) {
    printf("heap corruption.\n");
    abort();
  }
#endif

  if (heap->fred_idx) {
    // we have to process the previously released block first.
    if (following_block(heap, block) == heap->fred_block) {
      // combine this block with next.
      heap->fred_idx += idx;
      heap->fred_block = block;
      set_block_tag(heap->fred_block, heap->fred_idx);
      return;
    } else if (following_block(heap, heap->fred_block) == block) {
      // combine preceding block with this.
      heap->fred_idx += idx;
      set_block_tag(heap->fred_block, heap->fred_idx);
      return;
    }

    block_t *merged_block = heap->fred_block;
    unsigned merged_size = heap->fred_idx;
    // try to merge the old block with its left neighbor.
    block_t *preceding = preceding_block(merged_block);
    if (preceding) {
      unsigned prec_used = is_block_used(preceding);
      if (!prec_used) {
        unsigned prec_size = get_block_tag(preceding);
        if (preceding != heap->last_block) {
          // remove preceding block from the free list.
          remove_from_free(heap, preceding, prec_size);
        }
        merged_block = preceding;
        merged_size += prec_size;
        set_block_tag(merged_block, merged_size);
      }
    }
    // try to merge the old block with its right neighbor.
    block_t *following = following_block(heap, merged_block);
    if (following) {
      unsigned foll_used = is_block_used(following);
      if (!foll_used) {
        unsigned foll_size = get_block_tag(following);
        if (following != heap->last_block) {
          // remove followning block from the free list.
          remove_from_free(heap, following, foll_size);
        } else {
          heap->last_block = merged_block;
        }
        merged_size += foll_size;
        set_block_tag(merged_block, merged_size);
      }
    }
    // add the result to the free list, unless we merged with the last_block.
    if (merged_block != heap->last_block) {
      add_to_free(heap, merged_block, merged_size);
    } else {
      heap->last_idx = merged_size;
    }
  }
  
  // block was not combined, so lets make it the new 'previously released'.
  set_block_tag(block, idx); // marks the block free.
  heap->fred_idx = idx;
  heap->fred_block = block;
}

void heap_free_large(largeheap_t *heap, void* ptr) {
  block_t* block = (block_t*)ptr;
  mark_block_free(block);
  block->next = heap->free;
  heap->free = block;
}
