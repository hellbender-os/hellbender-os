#include <stddef.h>
#include <string.h>

#include <kernel/kernel.h>
#include <kernel/kstdlib.h>
#include <kernel/kstdio.h>
#include <kernel/mem.h>
#include <kernel/mmap.h>

#define MEM_MAX_RANGES 16

/* We keep free pages in a "linked directory list".
 * Each directory contains up to DIRECTORY_SIZE free pages.
 * directories form a single linked list.
 * Two top-most entries of the linked list are always mapped
 * at fixed virtual addresses.
 * When those two directories become full/emptry, we either
 * push a new directory into the list, or pop one from the list.
 */
struct __attribute__((packed)) directory_header {
  uintptr_t this; // physical address of this directory.
  uintptr_t next; // physical address of the next directory.
  unsigned free; // number of free pages in the pages array.
};

#define DIRECTORY_SIZE ((PAGE_SIZE-sizeof(struct directory_header))/sizeof(uintptr_t))

// Must have: sizeof(free_directory_t) == PAGE_SIZE
typedef struct free_directory {
  struct directory_header header;
  uintptr_t pages[DIRECTORY_SIZE]; // physical addresses of free pages.
} __attribute__((packed)) free_directory_t;

typedef struct mem {
  // list of the free page ranges:
  struct range {
    uintptr_t base;
    size_t size;
  } ranges[MEM_MAX_RANGES];
  unsigned range; // range currently in use.
  uintptr_t range_base; // data for the current range for faster access.
  size_t range_size;

  size_t total_memory;
  size_t available_memory;

  // two first free page directories of the linked list.
  free_directory_t* first;
  free_directory_t* second;
  
} mem_t;
mem_t mem;

// These allocation provide us the first two directory pages.
// We also keep mapping the head of the links to these virtual addresses.
uint8_t mem_free_pages1[PAGE_SIZE] __attribute__((aligned(PAGE_SIZE)));
uint8_t mem_free_pages2[PAGE_SIZE] __attribute__((aligned(PAGE_SIZE)));

/**
 * Initializes the free memory information based on GRUB memory maps.
 * Kernel and core service module area are excluded.
 */
void mem_early_initialize(memory_map_t *memory_map, unsigned map_elements,
                          uintptr_t kernel_bottom, uintptr_t kernel_top,
                          uintptr_t core_bottom, uintptr_t core_top) {
  memset(&mem, 0, sizeof(mem));
  memset(mem_free_pages1, 0, PAGE_SIZE);
  memset(mem_free_pages2, 0, PAGE_SIZE);
  
  mem.first = (free_directory_t*)mem_free_pages1;
  mem.second = (free_directory_t*)mem_free_pages2;
  mem.first->header.this = (uintptr_t)mem_free_pages1;
  mem.first->header.next = (uintptr_t)mem_free_pages2;
  mem.second->header.this = (uintptr_t)mem_free_pages2;

  // round the input values to page boundaries.
  kernel_bottom -= kernel_bottom % PAGE_SIZE;
  if (kernel_top % PAGE_SIZE) kernel_top += PAGE_SIZE - kernel_top % PAGE_SIZE;
  core_bottom -= core_bottom % PAGE_SIZE;
  if (core_top % PAGE_SIZE) core_top += PAGE_SIZE - core_top % PAGE_SIZE;

  // process memory map information provided by GRUB:
  unsigned j = 0;
  for (unsigned i = 0; i < map_elements; ++i) {
    uintptr_t base = memory_map[i].base_addr_low;
    size_t size = memory_map[i].length_low;
    mem.total_memory += size;

    // first 4MB is recorded as free pages.
    while (base < TABLE_SIZE && size >= PAGE_SIZE) {
      // skip pages reserved for kernel and core service.
      uintptr_t top = base+size;
      if ((top <= kernel_bottom && top <= core_bottom)
          || (base >= kernel_top && base >= core_top)) {
        mem_free_page(base);
      }
      base += PAGE_SIZE;
      size -= PAGE_SIZE;
    }
    // anything above 4MB is added as a range.
    if (size >= PAGE_SIZE) {
      if (j >= MEM_MAX_RANGES) {
        kprintf("maximum number of memory ranges exceeded!\n");
        kabort();
      }
      mem.ranges[j].base = base;
      mem.ranges[j].size = size;
      mem.available_memory += size;
      ++j;
    }
  }

  kprintf("%u bytes of memory, %u bytes free.\n",
          (unsigned)mem.total_memory, (unsigned)mem.available_memory);
}

uintptr_t mem_alloc_page() {
  // if current directories are empty, try to get a pages from ranges:
  if (mem.first->header.free == 0 && mem.second->header.free == 0
      && mem.range_size) {
    uintptr_t page = mem.range_base;
    mem.range_base += PAGE_SIZE;
    mem.range_size -= PAGE_SIZE;
    // switch to the next range if this became empty.
    if (mem.range_size == 0 && mem.range < MEM_MAX_RANGES) {
      ++mem.range;
      mem.range_base = mem.ranges[mem.range].base;
      mem.range_size = mem.ranges[mem.range].size;
    }
    return page;
  }
  
  // try to get free pages from the visible directories:
  if (mem.first->header.free) {
    return mem.first->pages[--mem.first->header.free];
  } else if (mem.second->header.free) {
    return mem.second->pages[--mem.second->header.free];
  }
  
  // now we don't have any choise but to pop the linked list.
  if (mem.second->header.next) {
    // the first empty page can be returned to the application.
    uintptr_t page = mem.first->header.this;

    // the new directory page is be mapped to replace of the old one.
    mmap_map_page(mem.first, mem.second->header.next, MMAP_ATTRIB_KERNEL);

    // and the old "second" becomes the new "first".
    free_directory_t* tmp = mem.first;
    mem.first = mem.second;
    mem.second = tmp;

    return page;
  }

  // we have run out of memory..
  kprintf("Out of memory!\n");
  kabort();
}

void mem_free_page(uintptr_t page) {
  // TODO: use a magic token to check that the page is not mapped!
  
  // try to put the page into the second directory.
  if (mem.second->header.free < DIRECTORY_SIZE) {
    mem.second->pages[mem.second->header.free++] = page;
    return;
  }
  
  // try to put the page into the first directory.
  if (mem.first->header.free < DIRECTORY_SIZE) {
    mem.first->pages[mem.first->header.free++] = page;
    return;
  }

  // we have to push the linked list.
  // the new page becomes our new directory, replacing the second directory.
  mmap_map_page(mem.second, page, MMAP_ATTRIB_KERNEL);
  mem.second->header.this = page;
  mem.second->header.next = mem.first->header.this;
  mem.second->header.free = 0;

  // and the old "first" becomes the new "second".
  free_directory_t* tmp = mem.first;
  mem.first = mem.second;
  mem.second = tmp;
}

void* mem_alloc_mapped(void *virtual, size_t size) {
  if (size % PAGE_SIZE) size += PAGE_SIZE - size % PAGE_SIZE;
  for (; size; size -= PAGE_SIZE, virtual += PAGE_SIZE) {
    uintptr_t physical = mem_alloc_page();
    mmap_map_page(virtual, physical, MMAP_ATTRIB_USER);
  }
  return virtual;
}

