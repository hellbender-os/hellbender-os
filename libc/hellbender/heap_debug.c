
#include <kernel/kernel.h>
#include <kernel/mem.h>
#include <hellbender/heap.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "heap_impl.h"

void heap_debug_small(smallheap_t *heap) {
  /*
  // sanity checking:
  //  for each block in chain:
  //   if block if reserved,
  //    check that it is not in any free lists.
  //    check that it is in the returned pointers.
  //   if block is !reserved,
  //    check that it is in the correct free list.
  //    check that it is not in any other free lists.
  block_t *b = ((block_t*)heap->wilderness->base)+1;
  while (b && get_block_size(b)) {
    if (b == heap->last_block || b == heap->fred_block || is_block_used(b)) {
      for (int i = 0; i < 131; ++i) {
        block_t *ptr = heap->free[i];
        for (; ptr; ptr = ptr->next) {
          if (b == ptr) {
            printf("ALLOCATED BLOCK IN FREE LIST!\n");
            abort();
          }
        }
      }
      
    } else {
      unsigned idx = get_block_size(b);
      if (idx > 129) idx = 130;
      for (unsigned i = 0; i < 131; ++i) {
        if (i == idx) continue;
        block_t *ptr = heap->free[i];
        for (; ptr; ptr = ptr->next) {
          if (b == ptr) {
            printf("FREED BLOCK IN WRONG FREE LIST!\n");
            abort();
          }
        }
      }
      block_t *ptr = heap->free[idx];
      int found = 0;
      for (; ptr; ptr = ptr->next) {
        if (b == ptr) {
          found = 1;
        }
      }
      if (!found) {
        printf("FREED BLOCK NOT IN FREE LIST!\n");
        abort();
      }
    }
    b = following_block(heap, b);
  }
  */
}
