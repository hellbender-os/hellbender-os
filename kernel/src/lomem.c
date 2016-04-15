#include "lomem.h"
#include "kernel.h"
#include "multiboot.h"
#include "config.h"
#include "list.h"
#include "spin.h"
#include "page.h"

#define FREE_TABLE_HEADER(table) ((struct free_table*)(((uintptr_t)(table)) + TABLE_SIZE - PAGE_SIZE))
#define SPLIT_TABLE_HEADER(table) ((struct split_table*)(((uintptr_t)(table)) + TABLE_SIZE - PAGE_SIZE))
#define TABLE_ADDRESS(table) ((void*)(((uintptr_t)table) & TABLE_ADDRESS_MASK))

struct free_table { // last page of a free table.
  LIST_ITEM;
};

struct split_table { // last page of a split table.
  LIST_ITEM;
  unsigned n_free;
  list_t free_pages; // struct free_page
};

struct free_page {
  LIST_ITEM;
};
        
struct lomem {
  spinlock_raw_t free_lock; // protects free_tables.
  list_t free_tables; // struct free_table

  spinlock_raw_t split_lock; // protects split_tables; split_table::n_free, split_table::free_pages.
  list_t split_tables; // struct split_table
};

static struct lomem lomem;

// 0 for existing, <0 unavailable
static int lomem_check_page_exists(uintptr_t page, uintptr_t size) {
  uintptr_t page_top = page + size;

  // check if the memory page exists:
  for (unsigned i = 0; i < multiboot_data.n_mem_maps; ++i) {
    memory_map_t *map = multiboot_data.mem_maps + i;
    uintptr_t mem_bottom = ((uintptr_t)map->base_addr_low) 
      + (((uintptr_t)map->base_addr_high)<<32);
    uintptr_t mem_size = ((uint64_t)map->length_low) 
      + (((uint64_t)map->length_high)<<32);
    uintptr_t mem_top = mem_bottom + mem_size;
    if (page >= mem_bottom && page_top <= mem_top) return 0;
  }
  return -1;
}

// 0 for free, >0 for allocated
static int lomem_check_page_free(uintptr_t page) {
  // special handling of the first page (due to bios usage).
  if (page == 0) return -1;
  // special handling of the last page of the first table (due to kernel load address hack).
  if (page == (TABLE_SIZE - PAGE_SIZE)) return 0;

  int state = lomem_check_page_exists(page, PAGE_SIZE);
  uintptr_t page_top = page + PAGE_SIZE;

  // check if page is used by a module:
  for (unsigned k = 0; k < multiboot_data.n_modules; ++k) {
    module_t *m = multiboot_data.modules + k;
    uintptr_t mod_bottom = page_round_down(m->mod_start);
    uintptr_t mod_top = page_round_up(m->mod_end);
    if (page >= mod_bottom && page_top <= mod_top) state = 1;
  }
  // check if page is used by the kernel:
  for (unsigned k = 0; k < multiboot_data.n_headers; ++k) {
    Elf32_Shdr *h = multiboot_data.headers + k;
    uintptr_t mod_bottom = page_round_down(h->sh_addr);
    uintptr_t mod_top = page_round_up(h->sh_addr + h->sh_size);
    if (page >= mod_bottom && page_top <= mod_top) state = 1;
  }
  return state;
}

void lomem_init() {
  list_init(&lomem.free_tables);
  list_init(&lomem.split_tables);

  // check the status of every table:
  for (uintptr_t table_base = multiboot_data.memory_bottom; 
       table_base < multiboot_data.memory_top; 
       table_base += TABLE_SIZE) {
    uintptr_t table_top = table_base + TABLE_SIZE;

    // check if the whole table is trivially free.
    if (table_base > multiboot_data.allocated_top && 
        lomem_check_page_exists(table_base, TABLE_SIZE) == 0) {
        // completely free table.
        struct free_table* free = FREE_TABLE_HEADER(kernel_p2v(table_base));
        LIST_INSERT(&lomem.free_tables, free);
        continue;
    }

    // last table page must be free because that is used as table header.
    uintptr_t last_page = table_top - PAGE_SIZE;
    if (lomem_check_page_free(last_page) == 0) {
      // check free pages in the table.
      unsigned n_free = 0;
      for (uintptr_t page = table_base; page < table_top; page += PAGE_SIZE) {
        if (lomem_check_page_free(page) == 0) ++n_free;
      }
      if (n_free == (TABLE_SIZE / PAGE_SIZE)) {
        // completely free table.
        struct free_table* free = FREE_TABLE_HEADER(kernel_p2v(table_base));
        LIST_INSERT(&lomem.free_tables, free);
      } else if (n_free > 1) {
        // partially free table.
        struct split_table* split = SPLIT_TABLE_HEADER(kernel_p2v(table_base));
        split->n_free = n_free - 1;
        list_init(&split->free_pages);
        for (uintptr_t page = table_base; page < last_page; page += PAGE_SIZE) {
          if (lomem_check_page_free(page) == 0) {
            LIST_INSERT(&split->free_pages, (struct free_page*)kernel_p2v(page));
          }
        }
        LIST_INSERT(&lomem.split_tables, split);
      }
    }
  }
}

uintptr_t lomem_alloc_4k() {
  { // try to find a split table.
    SPIN_GUARD_RAW(lomem.split_lock);
    struct split_table* split = LIST_FIRST(&lomem.split_tables, struct split_table);
    if (split) {
      struct free_page* page = LIST_FIRST(&split->free_pages, struct free_page);
      LIST_REMOVE(page);
      if (--(split->n_free) == 0) {
        LIST_REMOVE(split);
      }
      return kernel_v2p(page);
    }
  }

  return lomem_alloc_pages(1);
}

uintptr_t lomem_alloc_2M() {
  // find a free table.
  SPIN_GUARD_RAW(lomem.free_lock);
  struct free_table* free = LIST_FIRST(&lomem.free_tables, struct free_table);
  if (free) {
    LIST_REMOVE(free);
    return kernel_v2p(TABLE_ADDRESS(free));
  }

  // out of memory.
  return 0;
}

uintptr_t lomem_alloc_pages(unsigned count) {
  if (count >= (TABLE_SIZE/PAGE_SIZE)) kernel_panic();

  // to allocate N pages, we always split a free table.
  struct free_table* free = 0;
  {
    SPIN_GUARD_RAW(lomem.free_lock);
    free = LIST_FIRST(&lomem.free_tables, struct free_table);
    if (free) {
      LIST_REMOVE(free);
    }
  }
  if (free) {
    struct split_table* split = (struct split_table*)free;
    // add all but the N first and last table pages.
    // last page is the split table header.
    // first page(s) will be returned to caller.
    split->n_free = 0;
    list_init(&split->free_pages);
    uint8_t* base = (uint8_t*)TABLE_ADDRESS(free);
    for (uintptr_t offset = count*PAGE_SIZE; offset < (TABLE_SIZE - PAGE_SIZE); offset += PAGE_SIZE) {
      struct free_page* page = (struct free_page*)(base + offset);
      LIST_INSERT(&split->free_pages, page);
      ++split->n_free;
    }
    if (split->n_free) {
      SPIN_GUARD_RAW(lomem.split_lock);
      LIST_INSERT(&lomem.split_tables, split);
    }
    return kernel_v2p(base);
  }

  // out of memory.
  return 0;
}

void lomem_free_2M(uintptr_t page) {
  struct free_table* free = FREE_TABLE_HEADER(kernel_p2v(page));
  SPIN_GUARD_RAW(lomem.free_lock);
  LIST_INSERT(&lomem.free_tables, free);
}

void lomem_free_pages(uintptr_t page, unsigned count) {
  struct free_table* free = 0;
  uint8_t* ptr = (uint8_t*)kernel_p2v(page);
  void* table = TABLE_ADDRESS(ptr);
  struct split_table* split = SPLIT_TABLE_HEADER(table);
  {
    SPIN_GUARD_RAW(lomem.split_lock);
    if (split->n_free == 0) {
      LIST_INSERT(&lomem.split_tables, split);
    }
    split->n_free += count;
    if (split->n_free == ((TABLE_SIZE - PAGE_SIZE) / PAGE_SIZE)) {
      // when all pages are free, the table is free.
      LIST_REMOVE(split);
      free = (struct free_table*)split;
    } else {
      for (unsigned i = 0; i < count; ++i, ptr += PAGE_SIZE) {
        LIST_INSERT(&split->free_pages, (struct free_page*)ptr);
      }
    }
  }

  if (free) {
    SPIN_GUARD_RAW(lomem.free_lock);
    LIST_INSERT(&lomem.free_tables, free);
  }
}

void lomem_free_4k(uintptr_t page) {
  lomem_free_pages(page, 1);
}
