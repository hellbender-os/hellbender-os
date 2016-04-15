#include "heap.h"
#include "lomem.h"
#include "kernel.h"
#include "config.h"
#include "cpu.h"
#include "spin.h"

struct heap_header {
  unsigned size; // size of the whole block; user data area is size - sizeof(unsigned).
};

struct heap_block {
  struct heap_header hdr;
  struct heap_block *next;
} __attribute__ ((packed));

INLINE struct heap_block* HEAP_PTR2BLOCK(void *ptr) {
  return ((struct heap_block*)(((uintptr_t)(ptr)) - sizeof(struct heap_header)));
}

INLINE void* HEAP_BLOCK2PTR(struct heap_block*blk) {
  return ((void*)(((uintptr_t)(blk)) + sizeof(struct heap_header)));
}

struct heap_wilderness {
  union {
    struct {
      spinlock_raw_t lock;
      void *ptr;
      unsigned size;
    } s;
    uint8_t dummy[CACHE_LINE];
  } u;
} heap_wilderness __attribute__ ((aligned(CACHE_LINE)));

void* heap_alloc(unsigned bytes) {
  if (bytes > HEAP_MAX_ALLOC) kernel_panic();
  // try to allocate from per-cpu free list.
  unsigned idx = (bytes + sizeof(struct heap_header) - 1) / CACHE_LINE;
  struct heap_block **list = cpu.heap.free_lists + idx;
  struct heap_block *block = *list;
  if (block) {
    *list = block->next;
    return HEAP_BLOCK2PTR(block);
  } else {
    // must allocate from wilderness.
    unsigned block_size = CACHE_LINE * (idx + 1);
    SPIN_GUARD_RAW(guard, heap_wilderness.u.s.lock);
    if (heap_wilderness.u.s.size < block_size) {
      // more room needed, old unused wilderness is added as a free block.
      if (heap_wilderness.u.s.size) {
        unsigned w_idx = heap_wilderness.u.s.size / CACHE_LINE;
        struct heap_block **w_list = cpu.heap.free_lists + w_idx;
        struct heap_block *w_block = HEAP_PTR2BLOCK(heap_wilderness.u.s.ptr);
        w_block->hdr.size = heap_wilderness.u.s.size;
        w_block->next = *w_list;
        *w_list = w_block;
        heap_wilderness.u.s.size = 0;
      }
      // ensure that there is room for block size before the first block.
      heap_wilderness.u.s.ptr = kernel_p2v(lomem_alloc_2M());
      if (heap_wilderness.u.s.ptr) {
        heap_wilderness.u.s.size = TABLE_SIZE - CACHE_LINE;
        heap_wilderness.u.s.ptr += CACHE_LINE;
      }
    }
    if (heap_wilderness.u.s.size >= block_size) {
      block = HEAP_PTR2BLOCK(heap_wilderness.u.s.ptr);
      block->hdr.size = block_size;
      heap_wilderness.u.s.ptr += block_size;
      heap_wilderness.u.s.size -= block_size;
      return HEAP_BLOCK2PTR(block);
    } else kernel_panic();
  }
}

void heap_free(void* ptr) {
  struct heap_block* block = HEAP_PTR2BLOCK(ptr);
  unsigned idx = block->hdr.size / CACHE_LINE - 1;
  block->next = cpu.heap.free_lists[idx];
  cpu.heap.free_lists[idx] = block;
}
