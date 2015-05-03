#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <kernel/kernel.h>
#include <kernel/mem.h>

/*
  // round to next page size.
  size = size + PAGE_SIZE - 1;
  size -= size % PAGE_SIZE;
  if (memory->early_memory_size < size) {
    printf("Out of early kernel memory!\n");
    abort();
  }
 */

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

static inline void set_ds_all() {
    asm ("push %%ds;"
         "mov %%ax, %%dx;"
         : // no output
         : "a"(SEL_ALL_DATA)
         : "memory"
         );
}

static inline void reset_ds() {
    asm ("pop %%ds;"
         : // no output
         : // no input
         : "memory"
         );
}

void mem_early_initialize() {
  memset(&mem, 0, sizeof(mem));
  memset(&mem_page_directory, 0, PAGE_SIZE);
  memset(&mem_page_table_0, 0, PAGE_SIZE);
  memset(&mem_page_table_1, 0, PAGE_SIZE);

  uint32_t *page_dir = (uint32_t*)mem_page_directory;

  // map the first two page tables for DS and CS segments.
  page_dir[0] = ((uint32_t)(&mem_page_table_0)) | 3;
  page_dir[CS_BASE >> 22] = ((uint32_t)(&mem_page_table_1)) | 3;
  
  // recursive page directory is mapped to the last 4MB.
  // note that access requires SEL_ALL_DATA selector.
  page_dir[1023] = ((uint32_t)page_dir) | 3;
  mem.page_tables = (uint32_t*)((uintptr_t)1023 * (uintptr_t)0x400000);
  mem.page_directory = (uint32_t*)((uintptr_t)(mem.page_tables)
                                   + (uintptr_t)1023 * (uintptr_t)0x1000);
}

void mem_early_finalize(memory_map_t *memory_map, unsigned map_elements) {
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
  printf("%u bytes available.\n", (unsigned)mem.free_size);
  
  // TODO: build a map of virtual memory holes.
}

void* mem_early_map_page(void* virtual, uintptr_t page, unsigned attributes) {
  uintptr_t address = (uintptr_t)virtual;
  // page directory and page tables are mapped as part of the kernel.
  if ((address % PAGE_SIZE) || (page % PAGE_SIZE)) {
    printf("Address and page must be page aligned!\n");
    abort();
  }
  uint32_t *page_dir = (uint32_t*)mem_page_directory;
  uint32_t page_dir_index = address >> 22;
  uint32_t page_table_index = (address >> 12) & 0x3ff;
  //printf("PDI = %u; PTI = %u; attr = %u\n",
  //	 (unsigned)page_dir_index, (unsigned)page_table_index,
  //	 (unsigned)attributes);
  if ((page_dir[page_dir_index] & 1) == 0) {
    printf("mem_early_map_page supports only the first 4MB region!\n");
    abort();
  }
  uint32_t *page_table = (uint32_t*)(page_dir[page_dir_index] & 0xfffff000);
  page_table[page_table_index] = (page & 0xfffff000) | attributes;
  asm volatile("invlpg (%0)" ::"r"(address) : "memory");
  return virtual;
}

void* mem_early_map(void* virtual, uintptr_t physical,
                    size_t size, unsigned attributes) {
  //printf("Mapping %u bytes\n", (unsigned)size);
  uintptr_t address = (uintptr_t)virtual;
  if ((address % PAGE_SIZE) || (physical % PAGE_SIZE)) {
    printf("Virtual and physical addresses must be page aligned!\n");
    abort();
  }
  for (uint32_t i = 0; i < size; i += PAGE_SIZE) {
    mem_early_map_page(virtual + i, physical + i, attributes);
  }
  return virtual;
}
void mem_early_enable_paging() {
#ifdef DEBUG
  printf("mem_early_enable_paging\n");
#endif
  
  asm ("mov %%eax, %%cr3;"
       "mov %%cr0, %%eax;"
       "or $0x80000000, %%eax;"
       "mov %%eax, %%cr0;"
       : /* no output registers */
       : "a"(&mem_page_directory)
       : 
       ); 
}

/*
void kernel_early_reset_page_directory() {
#ifdef DEBUG
  printf("kernel_early_reset_page_directory\n");
#endif
  
  asm ("mov %%cr3, %%eax;"
       "mov %%eax, %%cr3;"
       : // no output 
       : // no input 
       : "%eax"
       ); 
}
*/

uintptr_t mem_alloc_page() {
  //printf("Allocating %u bytes\n", (unsigned)size);
  if (mem.free_size < PAGE_SIZE) {
    printf("Out of memory pages!");
    abort();
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

  //printf("PDI = %u; PTI = %u; attr = %u\n",
  //	 (unsigned)page_dir_index, (unsigned)page_table_index,
  //	 (unsigned)attributes);

  uint32_t *page_table = NULL;
  uint32_t* page_dir = mem.page_directory;
  uint32_t* page_tables = mem.page_tables;
  set_ds_all();
  
  // ensure that the is a page table.
  if ((page_dir[page_dir_index] & 1) == 0) {
    uintptr_t new_table = mem_alloc_page();
    page_dir[page_dir_index] = new_table | 3;
    page_table = page_tables + page_dir_index * 0x400;
    memset(page_table, 0, PAGE_SIZE);
  } else {
    page_table = page_tables + page_dir_index * 0x400;
  }

  // set the page table entry.
  page_table[page_table_index] = (physical & 0xfffff000) | attributes;
  reset_ds();
  return virtual;
}

uintptr_t mem_unmap_page(void* virtual) {
  uintptr_t address = (uintptr_t)virtual;
  unsigned page_dir_index = address >> 22;
  unsigned page_table_index = (address >> 12) & 0x3ff;
  uintptr_t physical = 0;
  uint32_t* page_dir = mem.page_directory;
  uint32_t* page_tables = mem.page_tables;
  set_ds_all();
  if (page_dir[page_dir_index] & 1) {
    // get and clear the page table entry.
    uint32_t *page_table = page_tables + page_dir_index * 0x400;
    uint32_t value = page_table[page_table_index];
    page_table[page_table_index] = 0;
    if (value & 1) {
      physical = value & 0xfffff000;
      //printf("unmapping PDI=%x; PTI=%x, was %x\n",
      //       (unsigned)page_dir_index, (unsigned)page_table_index,
      //       (unsigned)physical);
      asm volatile("invlpg (%0)" ::"r"(address) : "memory");
    }
  }
  reset_ds();
  return physical;
}

void* mem_map(void* virtual, uintptr_t physical,
              size_t size, unsigned attributes) {
  //printf("Mapping %u bytes\n", (unsigned)size);
  uintptr_t address = (uintptr_t)virtual;
  if ((address % PAGE_SIZE) || (physical % PAGE_SIZE)) {
    printf("Virtual and physical addresses must be page aligned!\n");
    abort();
  }
  for (uint32_t i = 0; i < size; i += PAGE_SIZE) {
    mem_map_page(virtual + i, physical + i, attributes);
  }
  return virtual;
}
