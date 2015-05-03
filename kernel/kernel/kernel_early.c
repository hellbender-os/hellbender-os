#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <kernel/kernel.h>
#include <kernel/kstdlib.h>
#include <kernel/kstdio.h>
#include <kernel/tty.h>
#include <kernel/mem.h>
#include <kernel/gdt.h>

#include <kernel/multiboot.h>
#include <kernel/elf32.h>

#define KERNEL_OFFSET 0x100000
#define DEBUG

/*
 * Convention:
 *  - uintptr_t refers to physical addresses.
 *  - void* refers to virtual addresses.
 *
 * DS, CS segments simulates "no-execute" flag.
 *  - DS base    0, maps all "no-execute" pages.
 *  - CS base 2^31, maps all "execute" pages.
 *  - limit = 2^31.
 *  - ES base    0, limit = 2^32.
 *
 * Kernel lives in the first page table.
 * Recursive page directory in the last 4MB of ES segment.
 *
 * kernel_early:
 *  - before page mapping, segments are set up.
 *  - remember reserved pages.
 *    - kernel ELF pages are reserved.
 *
 */

// memory for kernel_early initialization.
#define EARLY_MEMORY_SIZE (16*PAGE_SIZE)
uint8_t early_memory[EARLY_MEMORY_SIZE] __attribute__((aligned(PAGE_SIZE)));

#define MAX_MEMORY_MAP_ELEMENTS 32

typedef struct kernel_early {
  uintptr_t kernel_bottom;  // physical address range of kernel ELF.
  uintptr_t kernel_top;
  uintptr_t code_bottom; // physical address range of kernel code.
  uintptr_t code_top;

  // memory map information copied from multiboot header.
  unsigned memory_map_elements;
  memory_map_t memory_map[MAX_MEMORY_MAP_ELEMENTS];

} kernel_early_t;

static uintptr_t min(uintptr_t a, uintptr_t b) {
  return a < b ? a : b;
}

static uintptr_t max(uintptr_t a, uintptr_t b) {
  return a < b ? b : a;
}

void kernel_early_process_info(kernel_early_t *kernel,
                               multiboot_info_t *info) {
#ifdef DEBUG
  kprintf("kernel_early_process_info\n");
#endif

  // copy all available memory map elements into kernel structure.
  uintptr_t mm_end = info->mmap_addr + info->mmap_length;
  for (uintptr_t mm = info->mmap_addr; mm < mm_end; mm += sizeof(uint32_t)) {
    memory_map_t *map = (memory_map_t*)mm;
    /*
      kprintf("Memory .type=%u; .base=%u; .size=%u\n",
      (unsigned)map->type,
      (unsigned)map->base_addr_low,
      (unsigned)map->length_low);
    */
    if (map->type == 1) {
      if (kernel->memory_map_elements > MAX_MEMORY_MAP_ELEMENTS) {
        kprintf("Maximum number of memory map elements exceeded!");
        kabort();
      }
      memcpy(&kernel->memory_map[kernel->memory_map_elements++],
             map, sizeof(memory_map_t));
    }
    mm += map->size;
  }

  // scan all elf sections to find kernel memory.
  uintptr_t min_data = 0xffffffff;
  uintptr_t max_data = 0;
  uintptr_t min_code = 0xffffffff;
  uintptr_t max_code = 0;
  Elf32_Shdr *shdr = (Elf32_Shdr*)info->u.elf_sec.addr;
  //Elf32_Word shstrtab = shdr[info->u.elf_sec.shndx].sh_addr;
  for (unsigned i = 0; i < info->u.elf_sec.num; i++) {
    if (shdr[i].sh_addr) {
      min_data = min(min_data, shdr[i].sh_addr);
      max_data = max(max_data, shdr[i].sh_addr + shdr[i].sh_size);
      if (shdr[i].sh_flags & SHF_EXECINSTR) {
        min_code = min(min_code, shdr[i].sh_addr);
        max_code = max(max_code, shdr[i].sh_addr + shdr[i].sh_size);
      }
      //const char *name = &((const char *)shdr[i].sh_name)[shstrtab];
      //kprintf("Section %s at %u; %u bytes; type = %u\n",
      //     name,
      //     (unsigned)shdr[i].sh_addr,
      //     (unsigned)shdr[i].sh_size,
      //     (unsigned)shdr[i].sh_type);
    }
  }
  if (min_code < KERNEL_OFFSET) {
    kprintf("Kernel code loaded too low!");
    kabort();
  }

  // these are the limits for the whole kernel, and kernel code.
  kernel->kernel_bottom = min_data;
  kernel->kernel_top = max_data;
  kernel->code_bottom = min_code;
  kernel->code_top = max_code;
}

void kernel_early_init_paging(kernel_early_t *kernel) {
#ifdef DEBUG
  kprintf("kernel_early_init_paging\n");
#endif
  
  // map kernel code into CS section.
  mem_early_map((void*)(CS_BASE + kernel->code_bottom), kernel->code_bottom,
                kernel->code_top - kernel->code_bottom, 1);
  
  // map the whole kernel into DS section for now, we'll remove code later.
  mem_early_map((void*)kernel->kernel_bottom, kernel->kernel_bottom,
                kernel->kernel_top - kernel->kernel_bottom, 3);

  // and enable paging.
  mem_early_enable_paging();
}

void kernel_early_init_segments() {
#ifdef DEBUG
  kprintf("kernel_early_init_segments\n");
#endif

  gdt_entry_t gdt[SEL_COUNT] = {{0}};
  gdt[0] = (gdt_entry_t) { // 0x00 == NULL
    .base  = 0,
    .limit = 0,
    .type  = 0
  };
  gdt[SEL_KERNEL_CODE/8] = (gdt_entry_t) {
    .base  = CS_BASE,
    .limit = 0xffbfffff-CS_BASE, // last 4MB reserved for recursive page dir.
    .type  = 0x9A
  };
  gdt[SEL_KERNEL_DATA/8] = (gdt_entry_t) {
    .base  = 0x0,
    .limit = CS_BASE-1,
    .type  = 0x92
  };
  gdt[SEL_USER_CODE/8] = (gdt_entry_t) {
    .base  = CS_BASE,
    .limit = 0xffbfffff-CS_BASE,
    .type  = 0xF2
  };
  gdt[SEL_USER_DATA/8] = (gdt_entry_t) {
    .base  = 0x0,
    .limit = CS_BASE-1,
    .type  = 0x92
  };
  gdt[SEL_ALL_DATA/8] = (gdt_entry_t) {
    .base  = 0x0,
    .limit = 0xffffffff,
    .type  = 0x92
  };
  //gdt[5] = (gdt_entry_t) { // LTR(0x28)
  //  .base=kernel->tss,
  //  .limit=sizeof(task_switch_segment_t),
  //  .type=0x89
  //};
  for (int i = 0; i < SEL_COUNT; ++i) {
    gdt_early_set_entry(i*8, gdt[i]);
  }

  // switch to segmented mode.
  gdt_early_enable_segments();
}

void kernel_early_finalize(kernel_early_t *early) {
#ifdef DEBUG
  kprintf("kernel_early_finalize\n");
#endif
  
  // move the VGA text buffer.
  terminal_early_finalize();

  // remove executable pages from the DS segment.
  for (void* ptr = (void*)early->code_bottom;
       ptr < (void*)early->code_top; ptr += PAGE_SIZE) {
    mem_unmap_page(ptr);
  }
  
  // initialize mem allocator with free pages.
  mem_early_finalize(early->memory_map, early->memory_map_elements);
  // for each memory_map_element..
  // unmap early pages.
}

void kernel_early(uint32_t magic, multiboot_info_t *info) {
  mem_early_initialize();
  terminal_early_initialize();
#ifdef DEBUG
    kprintf("kernel_early\n");
#endif
    
  if (magic == MULTIBOOT_BOOTLOADER_MAGIC) {
    kernel_early_t kernel = {0};
    kernel_early_process_info(&kernel, info);
    kernel_early_init_paging(&kernel);
    kernel_early_init_segments(&kernel);
    kernel_early_finalize(&kernel);
  } else {
    kprintf("No multiboot!");
    kabort();
  }
#ifdef DEBUG
  kprintf("kernel_early DONE!\n");
#endif
}
