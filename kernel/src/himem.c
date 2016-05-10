#include "himem.h"
#include "kernel.h"
#include "multiboot.h"
#include "page.h"
#include "spin.h"
#include "lomem.h"
#include "log.h"

/* We keep free pages in a "linked directory list".
 * Each directory contains up to DIRECTORY_SIZE free pages.
 * directories form a single linked list.
 * When the two top-most directories become full/emptry, we either
 * push a new directory into the list, or pop one from the list.
 */
struct __attribute__((packed)) directory_header {
  struct directory_header* next; // address of the next directory.
  uintptr_t free; // number of free pages in the pages array.
};

#define DIRECTORY_SIZE ((PAGE_SIZE-sizeof(struct directory_header))/sizeof(uintptr_t))

// Must have: sizeof(free_directory_t) == PAGE_SIZE
typedef struct free_directory {
  struct directory_header header;
  uintptr_t pages[DIRECTORY_SIZE]; // physical addresses of free pages.
} __attribute__((packed)) free_directory_t;

typedef struct himem {
  // global locking for now. TODO: range specific locks, CPU specific dirs.
  spinlock_raw_t lock;

  // list of the free page ranges:
  struct range {
    uintptr_t base;
    uint64_t size;
  } ranges[MAX_MEMORY_MAPS];
  unsigned range; // range currently in use.
  unsigned n_ranges;
  uintptr_t range_base; // data for the current range for faster access.
  uint64_t range_size;

  // two first free page directories of the linked list.
  free_directory_t* first;
  free_directory_t* second;
  
} himem_t;
himem_t himem;

/**
 * Sets up the free memory lists.
 * Initializes the free memory information based on GRUB memory maps.
 * Reserves all memory areas above 1GB.
 * Maps all memory to kernel virtual memory.
 */
void himem_init() {
  himem.first = (free_directory_t*)kernel_p2v(lomem_alloc_4k());
  himem.second = (free_directory_t*)kernel_p2v(lomem_alloc_4k());
  himem.first->header.next = &himem.second->header;
  himem.first->header.free = 0;
  himem.second->header.next = 0;
  himem.second->header.free = 0;

  // process memory map information provided by GRUB:
  for (unsigned i = 0; i < multiboot_data.n_mem_maps; ++i) {
    memory_map_t *map = multiboot_data.mem_maps + i;
    uintptr_t base = ((uintptr_t)map->base_addr_low) 
      + (((uintptr_t)map->base_addr_high)<<32);
    uintptr_t size = ((uint64_t)map->length_low) 
      + (((uint64_t)map->length_high)<<32);

    // avoid partially mapped 2M pages.
    if (base % TABLE_SIZE) {
      uint64_t delta = TABLE_SIZE - base % TABLE_SIZE;
      base += delta;
      size -= delta;
    }
    size -= size % TABLE_SIZE;

    // avoid memory below HIGH_MEMORY_LIMIT.
    if (size && base < HIGH_MEMORY_LIMIT) {
        if (base + size < HIGH_MEMORY_LIMIT) continue;
        size -= (HIGH_MEMORY_LIMIT - base);
        base = HIGH_MEMORY_LIMIT;
    }

    if (size) {
      // add the memory as 'free range'.
      himem.ranges[himem.n_ranges].base = base;
      himem.ranges[himem.n_ranges].size = size;
      ++himem.n_ranges;

      // and map the memory to kernel area.
      uintptr_t top = base + size;
      for (uintptr_t table = base; table < top; table += TABLE_SIZE) {
      }
    }
  }
  
  // start from the first range.
  himem.range_base = himem.ranges[0].base;
  himem.range_size = himem.ranges[0].size;
}

uintptr_t himem_alloc_page() {
  SPIN_GUARD_RAW(himem.lock);

  // if current directories are empty, try to get a page from ranges:
  if (himem.first->header.free == 0 && himem.second->header.free == 0
      && himem.range_size) {
    uintptr_t page = himem.range_base;
    himem.range_base += PAGE_SIZE;
    himem.range_size -= PAGE_SIZE;
    // switch to the next range if this became empty.
    if (himem.range_size == 0 && himem.range < himem.n_ranges) {
      ++himem.range;
      himem.range_base = himem.ranges[himem.range].base;
      himem.range_size = himem.ranges[himem.range].size;
    }
    return page;
  }
  
  // try to get free pages from the visible directories:
  if (himem.first->header.free) {
    return himem.first->pages[--himem.first->header.free];
  } else if (himem.second->header.free) {
    return himem.second->pages[--himem.second->header.free];
  }
  
  // now we don't have any choice but to pop the linked list.
  if (himem.second->header.next) {
    // the first empty page can be returned to the application.
    uintptr_t page = kernel_v2p(himem.first);

    // the old "second" becomes the new "first"
    // and a new directory page replaces the old "second".
    himem.first = himem.second;
    himem.second = (free_directory_t*)himem.second->header.next;

    return page;
  }

  log_error("himem", "alloc_page", "Out of memory");
}

void himem_free_page(uintptr_t page) {
  SPIN_GUARD_RAW(himem.lock);

  // TODO: use a magic token to check that the page is not mapped!
  if (page < HIGH_MEMORY_LIMIT) log_error("himem", "alloc_page", "Invalid argument");
  
  // try to put the page into the second directory.
  if (himem.second->header.free < DIRECTORY_SIZE) {
    himem.second->pages[himem.second->header.free++] = page;
    return;
  }
  
  // try to put the page into the first directory.
  if (himem.first->header.free < DIRECTORY_SIZE) {
    himem.first->pages[himem.first->header.free++] = page;
    return;
  }

  // we have to push the linked list.
  // and the old "first" becomes the new "second"
  // and the new page becomes our new "first" directory.
  himem.second = himem.first;
  himem.first = (free_directory_t*)kernel_p2v(page);
  himem.first->header.next = &himem.second->header;
  himem.first->header.free = 0;
}
