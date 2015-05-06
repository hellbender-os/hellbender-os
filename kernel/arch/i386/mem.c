#include <stddef.h>
#include <string.h>

#include <kernel/kernel.h>
#include <kernel/kstdlib.h>
#include <kernel/kstdio.h>
#include <kernel/mem.h>

typedef struct mem {
  // a range of free pages that we are consuming next.
  uintptr_t free_pages;
  size_t free_size;
  
  uint32_t *page_directory; // virtual address to the recursive page directory.
  uint32_t *page_tables; // virtual address to the recursive page tables.
} mem_t;
mem_t mem;

uint8_t mem_page_directory[PAGE_SIZE] __attribute__((aligned(PAGE_SIZE)));
uint8_t mem_page_table_0[PAGE_SIZE] __attribute__((aligned(PAGE_SIZE)));
uint8_t mem_page_table_1[PAGE_SIZE] __attribute__((aligned(PAGE_SIZE)));

extern unsigned long __force_order;
static inline void invalidate(void *virtual) {
  asm volatile("invlpg (%1)"
               : "=m"(__force_order)
               : "r"((uint32_t)virtual)
               );
}

static inline void invalidate_all() {
  asm volatile("mov %%cr3, %%eax;"
               "mov %%eax, %%cr3;"
               : "=m"(__force_order)
               :
               : "%eax"
               );
}

static inline unsigned set_ds_all() {
  unsigned old_ds;
  asm volatile("mov %%ds, %0;"
               "mov %2, %%ds;"
               "mov %2, %%es;"
               : "=&r"(old_ds), "=m"(__force_order)
               : "r"(SEL_ALL_DATA)
               : "memory"
               );
  return old_ds;
}

static inline void reset_ds(unsigned old_ds) {
  asm volatile("mov %1, %%ds;"
               "mov %1, %%es;"
               : "=m"(__force_order)
               : "r"(old_ds)
               : "memory"
               );
}

void mem_early_initialize() {
  memset(&mem, 0, sizeof(mem));
  memset(mem_page_directory, 0, PAGE_SIZE);
  memset(mem_page_table_0, 0, PAGE_SIZE);
  memset(mem_page_table_1, 0, PAGE_SIZE);

  uint32_t *page_dir = (uint32_t*)mem_page_directory;

  // map the first two page tables for DS and CS segments.
  page_dir[0] = ((uint32_t)(mem_page_table_0)) | MEM_ATTRIB_KERNEL;
  page_dir[CS_BASE >> 22] = ((uint32_t)(mem_page_table_1)) | MEM_ATTRIB_KERNEL;
  
  // recursive page directory is mapped to the last 4MB.
  // note that access requires SEL_ALL_DATA selector.
  page_dir[1023] = ((uint32_t)page_dir) | MEM_ATTRIB_KERNEL;
  mem.page_tables = (uint32_t*)((uintptr_t)1023 * (uintptr_t)0x400000);
  mem.page_directory = (uint32_t*)((uintptr_t)(mem.page_tables)
                                   + (uintptr_t)1023 * (uintptr_t)0x1000);
}

void mem_early_finalize(memory_map_t *memory_map, unsigned map_elements,
                        uintptr_t kernel_bottom, uintptr_t kernel_top,
                        uintptr_t core_bottom, uintptr_t core_top) {
  // hide page tables from the data segment.
  mem_unmap_page(&mem_page_directory);
  mem_unmap_page(&mem_page_table_0);
  mem_unmap_page(&mem_page_table_1);

  // TODO: build a map of free/allocated/reserved physical pages.
  for (unsigned i = 0; i < map_elements; ++i) {
    if (memory_map[i].length_low > mem.free_size) {
      mem.free_size = memory_map[i].length_low;
      mem.free_pages = memory_map[i].base_addr_low;
    }
  }

  // remove kernel pages from the allocator.
  kernel_bottom -= kernel_bottom % PAGE_SIZE;
  if (kernel_top % PAGE_SIZE) kernel_top += PAGE_SIZE - kernel_top % PAGE_SIZE;
  if (kernel_top > mem.free_pages) {
    mem.free_size -= (kernel_top - mem.free_pages);
    mem.free_pages = kernel_top;
  }

  // remove core server pages from the allocator.
  core_bottom -= core_bottom % PAGE_SIZE;
  if (core_top % PAGE_SIZE) core_top += PAGE_SIZE - core_top % PAGE_SIZE;
  if (core_top > mem.free_pages) {
    mem.free_size -= (core_top - mem.free_pages);
    mem.free_pages = core_top;
  }
  kprintf("%u bytes available, starting from %x.\n",
          (unsigned)mem.free_size, (unsigned)mem.free_pages);
  
  // TODO: build a map of virtual memory holes.
}

void* mem_early_map_page(void* virtual, uintptr_t page, unsigned attributes) {
  uintptr_t address = (uintptr_t)virtual;
  // page directory and page tables are mapped as part of the kernel.
  if ((address % PAGE_SIZE) || (page % PAGE_SIZE)) {
    kprintf("Address and page must be page aligned!\n");
    kabort();
  }
  uint32_t *page_dir = (uint32_t*)mem_page_directory;
  uint32_t page_dir_index = address >> 22;
  uint32_t page_table_index = (address >> 12) & 0x3ff;
  //kprintf("PDI = %u; PTI = %u; attr = %u\n",
  //	 (unsigned)page_dir_index, (unsigned)page_table_index,
  //	 (unsigned)attributes);
  if ((page_dir[page_dir_index] & 1) == 0) {
    kprintf("mem_early_map_page supports only the first 4MB region!\n");
    kabort();
  }
  uint32_t *page_table = (uint32_t*)(page_dir[page_dir_index] & 0xfffff000);
  page_table[page_table_index] = (page & 0xfffff000) | attributes;
  invalidate(virtual);
  return virtual;
}

void* mem_early_map(void* virtual, uintptr_t physical,
                    size_t size, unsigned attributes) {
  if (size % PAGE_SIZE) size += PAGE_SIZE - size % PAGE_SIZE;
  uintptr_t address = (uintptr_t)virtual;
  if ((address % PAGE_SIZE) || (physical % PAGE_SIZE)) {
    kprintf("Virtual and physical addresses must be page aligned!\n");
    kabort();
  }
  for (uint32_t i = 0; i < size; i += PAGE_SIZE) {
    mem_early_map_page(virtual + i, physical + i, attributes);
  }
  return virtual;
}

void mem_early_enable_paging() {
#ifdef DEBUG
  kprintf("mem_early_enable_paging\n");
#endif
  
  asm ("mov %%eax, %%cr3;"
       "mov %%cr0, %%eax;"
       "or $0x80000000, %%eax;"
       "mov %%eax, %%cr0;"
       : /* no output registers */
       : "a"(&mem_page_directory)
       ); 
}

uintptr_t mem_alloc_page() {
  if (mem.free_size < PAGE_SIZE) {
    kprintf("Out of memory pages!");
    kabort();
  }
  uintptr_t address = mem.free_pages;
  mem.free_pages += PAGE_SIZE;
  mem.free_size -= PAGE_SIZE;
  return address;
}

void* mem_map_page(void* virtual, uintptr_t physical, unsigned attributes) {
  uintptr_t address = (uintptr_t)virtual;
  unsigned page_dir_index = address >> 22;
  unsigned page_table_index = (address >> 12) & 0x3ff;

  //kprintf("Mapping virtual address = %x\n", (unsigned)virtual);
  //kprintf("PDI = %u; PTI = %u; attr = %u\n",
  //        (unsigned)page_dir_index, (unsigned)page_table_index,
  //        (unsigned)attributes);

  uint32_t* page_dir = mem.page_directory;
  uint32_t* page_tables = mem.page_tables;
  uint32_t *page_table = page_tables + page_dir_index * 0x400;
  
  // ensure that the is a page table.
  unsigned old_ds = set_ds_all();
  if ((page_dir[page_dir_index] & 1) == 0) {
    uintptr_t new_table = mem_alloc_page();
    page_dir[page_dir_index] = new_table | attributes;
    invalidate(page_table);
    memset(page_table, 0, PAGE_SIZE);
  }

  // set the page table entry.
  page_table[page_table_index] = (physical & 0xfffff000) | attributes;
  invalidate(virtual);
  reset_ds(old_ds);
  return virtual;
}

uintptr_t mem_unmap_page(void* virtual) {
  //kprintf("Unmapping virtual address = %x\n", (unsigned)virtual);
  uintptr_t address = (uintptr_t)virtual;
  unsigned page_dir_index = address >> 22;
  unsigned page_table_index = (address >> 12) & 0x3ff;

  uintptr_t physical = 0;
  uint32_t* page_dir = mem.page_directory;
  uint32_t* page_tables = mem.page_tables;
  uint32_t *page_table = page_tables + page_dir_index * 0x400;

  unsigned old_ds = set_ds_all();
  if (page_dir[page_dir_index] & 1) {
    // get and clear the page table entry.
    uint32_t value = page_table[page_table_index];
    page_table[page_table_index] = 0;
    if (value & 1) {
      physical = value & 0xfffff000;
      //kprintf("unmapping PDI=%x; PTI=%x, was %x\n",
      //       (unsigned)page_dir_index, (unsigned)page_table_index,
      //       (unsigned)physical);
      asm volatile("invlpg (%0)" ::"r"(address) : "memory");
    }
  }
  reset_ds(old_ds);
  return physical;
}

void* mem_map(void* virtual, uintptr_t physical,
              size_t size, unsigned attributes) {
  if (size % PAGE_SIZE) size += PAGE_SIZE - size % PAGE_SIZE;
  uintptr_t address = (uintptr_t)virtual;
  if ((address % PAGE_SIZE) || (physical % PAGE_SIZE)) {
    kprintf("Virtual and physical addresses must be page aligned!\n");
    kabort();
  }
  for (size_t i = 0; i < size; i += PAGE_SIZE) {
    mem_map_page(virtual + i, physical + i, attributes);
  }
  return virtual;
}

void mem_unmap(void* virtual, size_t size) {
  // round address downwards, take that into account in size, round size up.
  intptr_t address = (intptr_t)virtual;
  size += address % PAGE_SIZE;
  if (size % PAGE_SIZE) size += PAGE_SIZE - size % PAGE_SIZE;
  address -= address % PAGE_SIZE;
  for (void* ptr = (void*)address; size > 0;
       ptr += PAGE_SIZE, size -= PAGE_SIZE) {
    mem_unmap_page(ptr);
  }
}

void* mem_alloc_mapped(void *virtual, size_t size) {
  if (size % PAGE_SIZE) size += PAGE_SIZE - size % PAGE_SIZE;
  for (; size; size -= PAGE_SIZE, virtual += PAGE_SIZE) {
    uintptr_t physical = mem_alloc_page();
    mem_map_page(virtual, physical, MEM_ATTRIB_USER);
  }
  return virtual;
}

void* mem_map_table(void* virtual, uintptr_t page_table, unsigned attributes) {
  uintptr_t address = (uintptr_t)virtual;
  if (address & 0x3fffff) {
    kprintf("Page table must be mapped at table boundary.\n");
    kabort();
  }
  unsigned page_dir_index = address >> 22;
  uint32_t* page_dir = mem.page_directory;

  unsigned old_ds = set_ds_all();
  page_dir[page_dir_index] = page_table | attributes;
  reset_ds(old_ds);
  invalidate_all(); // TODO: or just all pages in the page table.
  return virtual;
}
