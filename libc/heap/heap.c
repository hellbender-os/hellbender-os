
#include <kernel/kernel.h>
#include <kernel/mem.h>
#include <sys/heap.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

wilderness_t default_wilderness;
tinyheap_t default_tinyheap;

void heap_grow(wilderness_t *wild) {
  uint8_t* new_top = wild->top + wild->step;
  if (new_top > wild->limit) {
    printf("Out of heap space!\n");
    abort();
  }
  mem_alloc_mapped(wild->top, wild->step);
  wild->top = new_top;
}

void wild_init(wilderness_t *wild, void *start, void *end, size_t alloc_size) {
  if (alloc_size % PAGE_SIZE) {
    printf("heap allocation size must be page aligned.\n");
    abort();
  }
  wild->base = (uint8_t*)start;
  wild->bottom = (uint8_t*)start;
  wild->top = (uint8_t*)start;
  wild->limit = (uint8_t*)end;
  wild->step = alloc_size;
  heap_grow(wild);
}

void tinyheap_init(tinyheap_t *heap, wilderness_t *wild) {
  memset(heap, 0, sizeof(tinyheap_t));
  heap->wilderness = wild;
}

void tinyheap_free(tinyheap_t *heap, void* ptr) {
  tinyblock_t* block = (tinyblock_t*)(((uint8_t*)ptr)-1);
  if (block->used.tag != 0xff) {
    printf("not a tinyheap block!\n");
    abort();
  }
  block->free.next = heap->some;
  heap->some = block;
}

void* tinyheap_malloc(tinyheap_t *heap, size_t size) {
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
        heap_grow(heap->wilderness);
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

void* tinyheap_realloc(tinyheap_t* heap, void* ptr, size_t size) {
  (void)(heap); (void)(ptr); (void)(size);
  return NULL;
}

// ================================
 
void heap_init(heap_t *heap, wilderness_t *wild) {
  memset(heap, 0, sizeof(heap_t));
  heap->wilderness = wild;
}

static inline unsigned is_block_used(block_t* block) {
  return (*(((uint32_t*)block)-1) & BLOCK_RESERVED) != 0;
}

static inline void mark_block_used(block_t* block) {
  *(((uint32_t*)block)-1) |= BLOCK_RESERVED;
}

static inline void mark_block_free(block_t* block) {
  *(((uint32_t*)block)-1) &= ~BLOCK_RESERVED;
}

static inline unsigned get_block_size(block_t* block) {
  return *(((uint32_t*)block)-1) & BLOCK_SIZE_MASK;
}

static inline void set_block_size(block_t* block, uint32_t size) {
  *(((uint32_t*)block)-1) = size;
  *((uint32_t*)(block + (size & BLOCK_SIZE_MASK) - 1)) = size;
}

static inline unsigned is_valid_block(block_t* block, unsigned size) {
  unsigned size2 = *((uint32_t*)(block + size - 1)) & BLOCK_SIZE_MASK;
  return size == size2;
}

static inline block_t* preceding_block(block_t *block) {
  unsigned preceding_size = *((uint32_t*)(block - 1)) & BLOCK_SIZE_MASK;
  if (preceding_size) return block - preceding_size;
  else return NULL;
}

static inline block_t* following_block(heap_t *heap, block_t *block) {
  unsigned block_size = get_block_size(block);
  block_t* following = block + block_size;
  if (following != heap->end_block) {
    return following;
  } else return NULL;
}

static inline void add_to_free(heap_t *heap, block_t* block, unsigned idx) {
  mark_block_free(block);
  if (idx > 129) idx = 130; // last free list contains all huge blocks.
  block->prev = NULL;
  block->next = heap->free[idx];
  if (block->next) block->next->prev = block;
  heap->free[idx] = block;
}

static inline void remove_from_free(heap_t* heap, block_t* block, unsigned idx) {
  if (idx > 129) idx = 130;
  if (block->prev) {
    block->prev->next = block->next;
  } else {
    heap->free[idx] = block->next;
  }
  if (block->next) {
    block->next->prev = block->prev;
  }
}

void heap_free(heap_t *heap, void* ptr) {
  block_t *block = (block_t*)ptr;
  unsigned idx = get_block_size(block);
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
      set_block_size(heap->fred_block, heap->fred_idx);
      return;
    } else if (following_block(heap, heap->fred_block) == block) {
      // combine preceding block with this.
      heap->fred_idx += idx;
      set_block_size(heap->fred_block, heap->fred_idx);
      return;
    }

    block_t *merged_block = heap->fred_block;
    unsigned merged_size = heap->fred_idx;
    // try to merge the old block with its left neighbor.
    block_t *preceding = preceding_block(merged_block);
    if (preceding) {
      unsigned prec_used = is_block_used(preceding);
      if (!prec_used) {
        unsigned prec_size = get_block_size(preceding);
        if (preceding != heap->last_block) {
          // remove preceding block from the free list.
          remove_from_free(heap, preceding, prec_size);
        }
        merged_block = preceding;
        merged_size += prec_size;
        set_block_size(merged_block, merged_size);
      }
    }
    // try to merge the old block with its right neighbor.
    block_t *following = following_block(heap, merged_block);
    if (following) {
      unsigned foll_used = is_block_used(following);
      if (!foll_used) {
        unsigned foll_size = get_block_size(following);
        if (following != heap->last_block) {
          // remove followning block from the free list.
          remove_from_free(heap, following, foll_size);
        } else {
          heap->last_block = merged_block;
        }
        merged_size += foll_size;
        set_block_size(merged_block, merged_size);
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
  set_block_size(block, idx); // marks the block free.
  heap->fred_idx = idx;
  heap->fred_block = block;
}

static inline void push_last_block_into_free(heap_t *heap) {
  if (heap->last_idx) {
    block_t* to_add = heap->last_block;
    unsigned add_size = heap->last_idx;
    add_to_free(heap, to_add, add_size);
    heap->last_idx = 0;
  }
}
 
void* heap_malloc(heap_t *heap, size_t size) {
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
      heap_grow(heap->wilderness);
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

void* heap_realloc(heap_t* heap, void* ptr, size_t size) {
  if (ptr == NULL) return heap_malloc(heap, size);
  if (size == 0) {
    heap_free(heap, ptr);
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
  void *new_ptr = heap_malloc(heap, size);
  size_t old_size = 8*jdx;
  memcpy(new_ptr, ptr, old_size < size ? old_size : size);
  return new_ptr;
}

void heap_sanity_check(heap_t *heap) {
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
}
