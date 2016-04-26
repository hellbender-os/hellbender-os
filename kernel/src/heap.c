#include "heap.h"
#include "lomem.h"
#include "kernel.h"
#include "config.h"
#include "cpu.h"
#include "spin.h"

struct heap_header {
  int64_t size; // size of the whole block == user data size + sizeof(struct heap_header).
};

struct heap_block {
  struct heap_header hdr;
  struct heap_block *next;
} __attribute__ ((packed));

INLINE struct heap_block* HEAP_PTR2BLOCK(void *ptr) {
  return ((struct heap_block*)(((uintptr_t)(ptr)) - sizeof(struct heap_header)));
}

INLINE void* HEAP_BLOCK2PTR(struct heap_block *blk) {
  return ((void*)(((uintptr_t)(blk)) + sizeof(struct heap_header)));
}

struct heap_wilderness {
  union {
    struct {
      spinlock_raw_t lock;
      void *ptr;
      unsigned size;
    };
    uint8_t dummy[CACHE_LINE];
  };
} heap_wilderness __attribute__ ((aligned(CACHE_LINE)));

static void* heap_large_alloc(size_t bytes) {
  unsigned n_pages = (bytes + sizeof(struct heap_header) + PAGE_SIZE - 1) / PAGE_SIZE;
  uintptr_t address = lomem_alloc_pages(n_pages);
  if (!address) kernel_panic();
  struct heap_block *block = (struct heap_block*)kernel_p2v(address);
  block->hdr.size = -(int)n_pages;
  return HEAP_BLOCK2PTR(block);
}

void* heap_alloc(size_t bytes) {
  if (bytes > HEAP_MAX_ALLOC) return heap_large_alloc(bytes);
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
    SPIN_GUARD_RAW(heap_wilderness.lock);
    if (heap_wilderness.size < block_size) {
      // more room needed, old unused wilderness is added as a free block.
      if (heap_wilderness.size) {
        unsigned w_idx = heap_wilderness.size / CACHE_LINE;
        struct heap_block **w_list = cpu.heap.free_lists + w_idx;
        struct heap_block *w_block = HEAP_PTR2BLOCK(heap_wilderness.ptr);
        w_block->hdr.size = heap_wilderness.size;
        w_block->next = *w_list;
        *w_list = w_block;
        heap_wilderness.size = 0;
      }
      // ensure that there is room for block size before the first block.
      heap_wilderness.ptr = kernel_p2v(lomem_alloc_2M());
      if (heap_wilderness.ptr) {
        heap_wilderness.size = TABLE_SIZE - CACHE_LINE;
        heap_wilderness.ptr += CACHE_LINE;
      }
    }
    if (heap_wilderness.size >= block_size) {
      block = HEAP_PTR2BLOCK(heap_wilderness.ptr);
      block->hdr.size = block_size;
      heap_wilderness.ptr += block_size;
      heap_wilderness.size -= block_size;
      return HEAP_BLOCK2PTR(block);
    } else kernel_panic();
  }
}

void heap_free(void* ptr) {
  struct heap_block* block = HEAP_PTR2BLOCK(ptr);
  if (block->hdr.size < 0) {
    lomem_free_pages(kernel_v2p(block), -block->hdr.size);
  } else {
    unsigned idx = block->hdr.size / CACHE_LINE - 1;
    block->next = cpu.heap.free_lists[idx];
    cpu.heap.free_lists[idx] = block;
  }
}
