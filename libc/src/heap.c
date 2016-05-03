#include "heap.h"

#include <hellbender/config.h>
#include <hellbender/syscall.h>
#include <hellbender/fifo.h>

#include <pthread.h>
#include <stdint.h>

/*
    Memory allocated in tables.
    A table is split into 256kB regions.
    Each region has an info structure at the end (footer table).

    Three kind of allocation blocks:
        Small block == fits into a slab.
        Medium block == fits into a region.
        Large block == requires multiple regions.

    A region is split into two halves:
        First half can be free, medium block, or large block tail.
        Second half can be free, medium block, a block list, or a slab.
        A region with a slab can be owned by a thread.
        (a slab is either owned by a thread, or it has allocated elements)

    Three kind of pointers.
        ptr % REGION_SIZE == 0  => pointer to a medium block in the first half.
        ptr % REGION_SIZE == 16 => pointer to a large block covering several regions.
        ptr % REGION_SIZE > 16  => a block, a block list item or slab item, see table footer.
*/

#define REGION_SIZE 0x40000 // each page table is split into regions.
#define MEDIUM_SIZE 1024    // blocks smaller than this use slab allocator.
#define LARGE_SIZE (REGION_SIZE-sizeof(struct region_header))
                            // blocks larger than this require several regions.
#define N_SLABS 63          // slab bins for sizes 16,32,48,...,976,992,1008

#define REGION_PTR_MASK (~(REGION_SIZE-1))

#define REGION_PTR(ptr) ((void*)(((uintptr_t)(ptr)) & REGION_PTR_MASK))

#define REGION_HEADER(ptr) ((struct region_header*)((((uintptr_t)(ptr)) & REGION_PTR_MASK) + REGION_SIZE - sizeof(struct region_header)))

#define TABLE_HEADER(ptr) ((struct region_header*)((((uintptr_t)(ptr)) & TABLE_VIRTUAL_MASK) + REGION_SIZE - sizeof(struct region_header)))

#define SLAB_HEADER(region) ((struct slab_header*)(((uintptr_t)(region)) - sizeof(struct slab_header)))

#define REGION_1ST_BLOCK    0x001
#define REGION_1ST_TAIL     0x002
#define REGION_1ST_MASK     0x00F

#define REGION_2ND_BLOCK    0x010
#define REGION_2ND_LIST     0x020
#define REGION_2ND_SLAB     0x040
#define REGION_2ND_MASK     0x0F0

#define REGION_OWNED        0x100  // thread owns this region.

struct slab_header {
  union {
    struct {
      void *first_free;       // pointer to first free element.
      uint32_t free_elements; // number of elements in the free list.
      uint32_t max_elements;  // total number of elements that fit into this slab.
      uint32_t element_size;  // size of elements in this slab.
      uint32_t tail;          // where to split the next element from.
      void *head;             // beginning of the slab data area.
    };
  };
} __attribute__((packed, aligned(32)));

struct region_header {
  fifo_item_t item;
  void *second;          // pointer to the second half.
  uint32_t flags;        // type and status of first and second halves.
  uint32_t free_regions; // the number of free regions in a table.
  pthread_mutex_t mutex;
};


struct global_heap {
  // fifo for each slab.
  fifo_t slabs[N_SLABS]; // non-empty slabs regions (not owned by any thread).
  fifo_t regions;        // free regions.
};

struct thread_heap {
  struct region_header *slabs[N_SLABS]; // non-empty slabs owned by the thread.
};

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static struct global_heap global_heap;
static struct thread_heap thread_heap; // TODO: make thread local!

static struct region_header* alloc_region() {
  struct region_header* region = 0;
  pthread_mutex_lock(&mutex);
  // try to get a free region from global heap.
  fifo_item_t *item = fifo_pop(&global_heap.regions);
  if (item) {
    region = fifo_container(item, struct region_header, item);
    --TABLE_HEADER(region)->free_regions;
  } else {
    // no regions, time to go fetch some from kernel.
    pthread_mutex_unlock(&mutex);
    uintptr_t table_base = syscall_alloc_tables(1);
    pthread_mutex_lock(&mutex);
    if (table_base) {
      // split the table into regions.
      // all but last are added as free regions, last one will be returned to caller.
      unsigned free_regions = TABLE_SIZE / REGION_SIZE - 1;
      for (uintptr_t region_offset = 0; region_offset < TABLE_SIZE; 
           region_offset += REGION_SIZE) {
        if (region) fifo_push(&global_heap.regions, &region->item);
        region = REGION_HEADER(table_base + region_offset);
        region->free_regions = free_regions;
        region->mutex = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
      }
    }
  }
  pthread_mutex_unlock(&mutex);
  return region;
};

static void free_region(struct region_header* region) {
  pthread_mutex_lock(&mutex);
  fifo_push(&global_heap.regions, &region->item);
  ++TABLE_HEADER(region)->free_regions;
  // TODO: if returning full tables back to OS, remember that caller hold region->lock!
  // this should return table address and the caller would release the lock and the table.
  pthread_mutex_unlock(&mutex);
}

INLINE void* slab_split(struct slab_header *slab) {
  slab->tail -= slab->element_size;
  if (slab->tail < slab->element_size) slab->tail = 0;
  return (void*)(((uintptr_t)slab->head) + slab->tail);
}

static void* alloc_small(size_t size) {
  unsigned slab_idx = (size / 16) - 1;
  struct region_header *region = 0;
  
  while ((region = thread_heap.slabs[slab_idx])) {
    struct slab_header *slab = SLAB_HEADER(region);
    // chopping the tail is fastest, because there is no locking involved.
    if (slab->tail) {
      return slab_split(slab);
    }
    // if tail is lost, we try to pop from the free list.
    void* free = 0;
    while (slab->first_free) {
      free = slab->first_free;
      void** ptr = (void**)free;
      void* next = *ptr;
      if (__sync_bool_compare_and_swap(ptr, free, next)) break;
    }
    if (free) {
      --slab->free_elements;
      return free;
    } else {
      // no tail, pop failed, we give this region away (unless someone just released).
      pthread_mutex_lock(&region->mutex);
      if (slab->free_elements == 0) {
        region->flags &= ~REGION_OWNED;
        thread_heap.slabs[slab_idx] = 0;
      }
      pthread_mutex_unlock(&region->mutex);
    }
  }
  
  // we need to get a new region for our slab.
  region = alloc_region();
  if (region) {
    region->flags = REGION_2ND_SLAB | REGION_OWNED;
    struct slab_header *slab = SLAB_HEADER(region);
    // by convention, first pointer cannot be 0 or 16.
    region->second = (void*)((uintptr_t)REGION_PTR(region) + 32); 
    slab->first_free = 0;
    slab->free_elements = 0;
    slab->head = region->second;
    slab->tail = (uint32_t)((uintptr_t)slab - (uintptr_t)slab->head);
    slab->max_elements = slab->tail / size;
    slab->element_size = size;
    thread_heap.slabs[slab_idx] = region;
    return slab_split(slab);
  }
  return 0;
}

static void* alloc_medium(size_t size) {
  (void)size;
  return 0;
}

static void* alloc_large(size_t size) {
  (void)size;
  return 0;
}

void* _heap_alloc(size_t size) { 
  // round size upwards to nearest 16.
  if (size) size = (size + 15) & -16;
  else size = 16;
  if (size < MEDIUM_SIZE) return alloc_small(size);
  else if (size > LARGE_SIZE) return alloc_large(size);
  else return alloc_medium(size);
}

static void free_first_half(void* ptr) {
  (void)ptr;
}

static void free_second_half(void* ptr) {
  struct region_header *region = REGION_HEADER(ptr);
  if (region->flags & REGION_2ND_SLAB) {
    struct slab_header *slab = SLAB_HEADER(region);
    void *free = 0;
    do {
      free = slab->first_free;
      *((void**)ptr) = free;
    } while (!__sync_bool_compare_and_swap(&slab->first_free, free, ptr));
    if (++slab->free_elements == slab->max_elements) {
      // when slab is fully freed, someone has to free the slab.
      pthread_mutex_lock(&region->mutex);
      if (!(region->flags & REGION_OWNED) && slab->free_elements == slab->max_elements) {
        region->flags &= ~REGION_2ND_MASK;
        if (region->flags == 0) {
          free_region(region);
        }
      }
      pthread_mutex_unlock(&region->mutex);
    }
  }

  else if (region->flags & REGION_2ND_LIST) {
    // TODO
  }
  
  else if (region->flags & REGION_2ND_BLOCK) {
    // TODO
  }
}

static void free_large(void* ptr) {
  (void)ptr;
}

void _heap_free(void* ptr) {
  uintptr_t address = (uintptr_t)ptr;
  uintptr_t rem = address % REGION_SIZE;
  if (rem == 0) free_first_half(ptr);
  else if (rem == 16) free_large(ptr);
  else free_second_half(ptr);
}
