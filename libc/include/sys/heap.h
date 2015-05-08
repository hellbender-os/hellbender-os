#ifndef _SYS_HEAP_H
#define _SYS_HEAP_H 1

#include <stddef.h>
#include <stdint.h>

typedef struct wilderness wilderness_t;
typedef struct wilderness {
    uint8_t *base; // original memory start address.
    uint8_t *bottom; // address where more physical memory will be allocated.
    uint8_t *top; // address where more physical memory will be allocated.
    uint8_t *limit; // address that may not be exceeded.
    size_t step; // how much more memory to allocate in one go.
} wilderness_t;
extern wilderness_t default_wilderness;

void wild_init(wilderness_t *wild, void *start, void *end, size_t alloc_size);

typedef struct tinyheap tinyheap_t;

// tinyheap:
//  allocates only 8 byte blocks, at most 7 data bytes.
//  => only one free list.
//  => no splitting or merging.
// tag == 0xff; no block in large heap can have this value.
typedef union __attribute__((packed)) tinyblock {
  struct __attribute__((packed)) {
    uint8_t tag;
    uint8_t data[7];
  } used;
  struct __attribute__((packed)) {
    uint8_t tag;
    union tinyblock* next;
    uint8_t filler[3];
  } free;
} tinyblock_t;

typedef struct tinyheap {
  wilderness_t *wilderness;

  tinyblock_t *some; // single linked list of free blocks.
  tinyblock_t *more; // an array of free blocks.
  unsigned more_size; // number of blocks in the array.
} tinyheap_t;
extern tinyheap_t default_tinyheap;

void tinyheap_init(tinyheap_t* heap, wilderness_t *wild);
void tinyheap_free(tinyheap_t* heap, void* ptr);
void* tinyheap_malloc(tinyheap_t* heap, size_t size);
void* tinyheap_realloc(tinyheap_t* heap, void* ptr, size_t size);

typedef struct heap heap_t;

// ================================

// large heap:
//   supports any allocation size.
//   blocks of 8*N bytes; N>1.
//   data size is 8*N-8.
//   data aligned at 8 byte boundaries.
// size tag:
//   block size / 8.
//   highest bit set if allocated.
// free blocks in a linked list.
//
// Allocated N-byte block:
//
//         v
// size(4) data(N-8) size(4)

// Released N-byte block:
//
//         v
// size(4) prev(4) next(4) unused(N-16) size(4)

#define BLOCK_RESERVED 0x80000000
#define BLOCK_SIZE_MASK 0x7fffffff

typedef struct block {
  struct block* prev;
  struct block* next;
} __attribute__((packed)) block_t;

typedef struct heap {
  wilderness_t *wilderness;
  block_t* end_block; // the block that is past the end of heap.

  // the block was was split; not yet in the free lists.
  unsigned last_idx;
  block_t* last_block;

  // the block that was released; not yet in the free lists.
  unsigned fred_idx;
  block_t* fred_block;

  // single linked lists of different sized blocks.
  block_t* free[131]; // max data size = idx*8-8; 1 < idx < 130.
  // last free list is for huge blocks of any size.

} heap_t;
extern heap_t default_heap;

void heap_init(heap_t* heap, wilderness_t *wild);
void heap_free(heap_t* heap, void* ptr);
void* heap_malloc(heap_t* heap, size_t size);
void* heap_realloc(heap_t* heap, void* ptr, size_t size);


#endif
