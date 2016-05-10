#include "lomem.h"
#include "kernel.h"
#include "multiboot.h"
#include "config.h"
#include "spin.h"
#include "page.h"
#include "log.h"

#include <hellbender/list.h>

// Memory is split into page table worth segments (2MB).
// The first or last page of each table is reserved to be used as a header, if needed.
// If page address < 8*TABLE_SIZE, we use the last page, else we use the first page.
// This way we get to use as much of the free space as possible without issues with
// the zero page & kernel (as long as kernel fits below that limit).

// describes a page table that is completely free.
struct free_table { // last page of a free table.
  list_item_t tables;
};

// describes a page table that is partially free.
struct split_table { // last page of a split table.
  list_item_t tables;
  uintptr_t o_unused; // offset from table bottom to the first unused page.
  unsigned n_unused;  // number of unused pages at the bottom of the table.
  unsigned n_free;    // number of reclaimed pages in the free_pages list.
  list_t free_pages;  // struct free_page
};

// describes a page that has been released.
struct free_page {
  list_item_t pages;
};
        
struct lomem {
  spinlock_raw_t free_lock;  // protects free_tables.
  list_t free_tables;        // list of struct free_table.

  spinlock_raw_t split_lock; // protects split_tables (and enclosed split tables).
  list_t split_tables;       // list of struct split_table.
};

static struct lomem lomem;

#define TOP_HEADER_LIMIT (8*TABLE_SIZE)

INLINE uintptr_t TABLE_ADDRESS(void *ptr) {
  return kernel_v2p(ptr) & TABLE_PHYSICAL_MASK;
}

INLINE void* TABLE_HEADER(void *ptr) {
  return (void*)((uintptr_t)ptr > (KERNEL_OFFSET + TOP_HEADER_LIMIT)
                 ? (((uintptr_t)ptr) & TABLE_VIRTUAL_MASK)
                 : (((uintptr_t)ptr) & TABLE_VIRTUAL_MASK) + TABLE_SIZE - PAGE_SIZE);
}

INLINE struct free_table* FREE_TABLE(void *ptr) {
  return (struct free_table*)(TABLE_HEADER(ptr));
}

INLINE struct split_table* SPLIT_TABLE(void *ptr) {
  return (struct split_table*)(TABLE_HEADER(ptr));
}

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
  // check the status of every table:
  for (uintptr_t table_base = multiboot_data.memory_bottom & TABLE_PHYSICAL_MASK; 
       table_base < multiboot_data.memory_top; table_base += TABLE_SIZE) {
    uintptr_t table_top = table_base + TABLE_SIZE;

    // check if the whole table is trivially free.
    if (table_base >= multiboot_data.allocated_top && 
        lomem_check_page_exists(table_base, TABLE_SIZE) == 0) {
        // completely free table.
        struct free_table* free = FREE_TABLE(kernel_p2v(table_base));
        list_insert(&lomem.free_tables, &free->tables);
        continue;
    }

    // header page must be free.
    struct split_table* split = SPLIT_TABLE(kernel_p2v(table_base));
    uintptr_t header_page = kernel_v2p(split);
    if (lomem_check_page_free(header_page) == 0) {
      split->n_unused = 0;
      split->o_unused = 0;
      split->n_free = 0;
      split->free_pages = (list_t)LIST_INIT;
      uintptr_t page = table_base;
      // find the first free page in the table.
      for (; page < table_top; page += PAGE_SIZE) {
        if (page == header_page || lomem_check_page_free(page) != 0) {
          split->o_unused += PAGE_SIZE;
        } else break;
      }
      // count the unused pages at the bottom of the table.
      for (; page < table_top; page += PAGE_SIZE) {
        if (page != header_page && lomem_check_page_free(page) == 0) ++split->n_unused;
        else break;
      }
      // add any other free pages to the 'free pages' list.
      for (; page < table_top; page += PAGE_SIZE) {
        if (page == header_page) continue;
        if (lomem_check_page_free(page) == 0) {
          ++split->n_free;
          struct free_page *free = (struct free_page *)kernel_p2v(page);
          list_insert(&split->free_pages, &free->pages);
        }
      }
      // tables with any free memory are added to the split table list.
      if (split->n_unused || split->n_free) {
        list_insert(&lomem.split_tables, &split->tables);
      }
    }
  }
}

uintptr_t lomem_alloc_4k() {
  { // try to find a split table.
    SPIN_GUARD_RAW(lomem.split_lock);
    list_item_t *table_item = list_first(&lomem.split_tables);
    if (table_item) {
      uintptr_t page = 0;
      struct split_table* split = list_container(table_item, struct split_table, tables);
      if (split->n_unused) {
        // use unused pages first.
        page = TABLE_ADDRESS(split) + split->o_unused;
        split->o_unused += PAGE_SIZE;
        --split->n_unused;
      } else {
        // any table in the list must have some free, thus n_free > 0 if n_unused == 0.
        list_item_t *page_item = list_first(&split->free_pages);
        page = kernel_v2p(list_container(page_item, struct free_page, pages));
        list_remove(page_item);
        --split->n_free;
      }
      if (split->n_unused == 0 && split->n_free == 0) {
        // split tables are removed when they get empty.
        list_remove(table_item);
      }
      return page;
    }
  }
  // if split tables are not available, get one from free tables.
  return lomem_alloc_pages(1);
}

uintptr_t lomem_alloc_2M() {
  // find a free table.
  SPIN_GUARD_RAW(lomem.free_lock);
  list_item_t *free_item = list_first(&lomem.free_tables);
  if (free_item) {
    list_remove(free_item);
    struct free_table* free = list_container(free_item, struct free_table, tables);
    return TABLE_ADDRESS(free);
  }

  // out of memory.
  return 0;
}

uintptr_t lomem_alloc_pages(unsigned count) {
  if (count >= (TABLE_SIZE/PAGE_SIZE)) log_error("lomem", "alloc_pages", "Invalid argument");

  // first check if split tables have enough unused pages.
  for (list_item_t *table_item = list_first(&lomem.split_tables); table_item;
       table_item = list_next(table_item)) {
    struct split_table* split = list_container(table_item, struct split_table, tables);
    if (split->n_unused >= count) {
      uintptr_t page = TABLE_ADDRESS(split) + split->o_unused;
      split->o_unused += count * PAGE_SIZE;
      split->n_unused -= count;
      if (split->n_unused == 0 && split->n_free == 0) {
        // split tables are removed when they get empty.
        list_remove(table_item);
      }
      return page;
    }
  }

  // if not found in split tables, we split a 2M free table.
  struct free_table* free = 0;
  {
    SPIN_GUARD_RAW(lomem.free_lock);
    list_item_t *free_item = list_first(&lomem.free_tables);
    if (free_item) {
      list_remove(free_item);
      free = list_container(free_item, struct free_table, tables);
    }
  }
  if (free) {
    struct split_table* split = (struct split_table*)free;
    split->n_free = 0;
    split->free_pages = (list_t)LIST_INIT;
    // first page(s) will be returned to caller
    // add the remaining page(s) are added as unused pages.
    uintptr_t page = TABLE_ADDRESS(split);
    split->n_unused = (TABLE_SIZE - PAGE_SIZE) / PAGE_SIZE - count;
    // ..but we need to dodge the header page.
    uintptr_t header_page = kernel_v2p(split);
    if (page == header_page) {
      page += PAGE_SIZE;
      split->o_unused = 2 * PAGE_SIZE;
    } else {
      split->o_unused = PAGE_SIZE;
    }
    if (split->n_unused) {
      SPIN_GUARD_RAW(lomem.split_lock);
      list_insert(&lomem.split_tables, &split->tables);
    }
    return page;
  }

  // out of memory.
  return 0;
}

void lomem_free_2M(uintptr_t page) {
  struct free_table* free = FREE_TABLE(kernel_p2v(page));
  SPIN_GUARD_RAW(lomem.free_lock);
  list_insert(&lomem.free_tables, &free->tables);
}

void lomem_free_pages(uintptr_t page, unsigned count) {
  struct free_table* free = 0;
  uint8_t* ptr = (uint8_t*)kernel_p2v(page);
  struct split_table* split = SPLIT_TABLE(ptr);
  {
    SPIN_GUARD_RAW(lomem.split_lock);
    if (split->n_free == 0) {
      list_insert(&lomem.split_tables, &split->tables);
    }
    split->n_free += count;
    if (split->n_free == ((TABLE_SIZE - PAGE_SIZE) / PAGE_SIZE)) {
      // when all pages are free, the table is free.
      list_remove(&split->tables);
      free = (struct free_table*)split;
    } else {
      for (unsigned i = 0; i < count; ++i, ptr += PAGE_SIZE) {
        list_insert(&split->free_pages, &((struct free_page*)ptr)->pages);
      }
    }
  }

  if (free) {
    SPIN_GUARD_RAW(lomem.free_lock);
    list_insert(&lomem.free_tables, &free->tables);
  }
}

void lomem_free_4k(uintptr_t page) {
  lomem_free_pages(page, 1);
}
