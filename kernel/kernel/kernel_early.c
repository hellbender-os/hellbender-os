#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <kernel/kernel.h>
#include <kernel/kstdlib.h>
#include <kernel/kstdio.h>
#include <kernel/tty.h>
#include <kernel/mem.h>
#include <kernel/gdt.h>
#include <kernel/tss.h>
#include <kernel/module.h>
#include <kernel/thread.h>

#include <kernel/multiboot.h>
#include <kernel/elf32.h>

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

#define MAX_MEMORY_MAP_ELEMENTS 32

typedef struct kernel_early {
  uintptr_t kernel_bottom;  // physical address range of kernel ELF.
  uintptr_t kernel_top;
  uintptr_t text_bottom; // physical address range of kernel code.
  uintptr_t text_top;

  uintptr_t core_bottom; // physical address range of core service.
  uintptr_t core_top;
  kernel_module_t core; // core segment sizes on disk image.

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
  uintptr_t min_text = 0xffffffff;
  uintptr_t max_text = 0;
  Elf32_Shdr *shdr = (Elf32_Shdr*)info->u.elf_sec.addr;
  //Elf32_Word shstrtab = shdr[info->u.elf_sec.shndx].sh_addr;
  for (unsigned i = 0; i < info->u.elf_sec.num; i++) {
    if (shdr[i].sh_addr) {
      min_data = min(min_data, shdr[i].sh_addr);
      max_data = max(max_data, shdr[i].sh_addr + shdr[i].sh_size);
      if (shdr[i].sh_flags & SHF_EXECINSTR) {
        min_text = min(min_text, shdr[i].sh_addr);
        max_text = max(max_text, shdr[i].sh_addr + shdr[i].sh_size);
      }
      //const char *name = &((const char *)shdr[i].sh_name)[shstrtab];
      //kprintf("Section %s at %u; %u bytes; type = %u\n",
      //     name,
      //     (unsigned)shdr[i].sh_addr,
      //     (unsigned)shdr[i].sh_size,
      //     (unsigned)shdr[i].sh_type);
    }
  }
  if (min_text < KERNEL_OFFSET) {
    kprintf("Kernel loaded too low!");
    kabort();
  }

  // these are the limits for the whole kernel, and kernel code.
  kprintf("Kernel found at %x - %x; code at %x - %x\n",
          (unsigned)min_data, (unsigned)max_data,
          (unsigned)min_text, (unsigned)max_text);
  kernel->kernel_bottom = min_data;
  kernel->kernel_top = max_data;
  kernel->text_bottom = min_text;
  kernel->text_top = max_text;

  // find the core service module.
  module_t *modules = (module_t*)info->mods_addr;
  for (unsigned i = 0; i < info->mods_count; ++i) {
    char *str = (char*)modules[i].string;
    kprintf("Module: %s\n", str);
    if (strcmp("--core", str) == 0) {
      kprintf("Found core service at %x - %x\n",
              (unsigned)modules[i].mod_start,
              (unsigned)modules[i].mod_end);
      kernel->core_bottom = modules[i].mod_start;
      kernel->core_top = modules[i].mod_end;
    }
  }
  if (!kernel->core_bottom) {
    kprintf("Core service not found!\n");
    kabort();
  }
  kernel_module_t* core_module = (kernel_module_t*)kernel->core_bottom;
  kernel->core = *core_module;

}

void kernel_early_init_paging(kernel_early_t *kernel) {
#ifdef DEBUG
  kprintf("kernel_early_init_paging\n");
#endif
  
  // map kernel code into CS section.
  mem_early_map((void*)(CS_BASE + kernel->text_bottom), kernel->text_bottom,
                kernel->text_top - kernel->text_bottom,
                MEM_ATTRIB_KERNEL);
  
  // map the whole kernel into DS section for now, we'll remove code later.
  mem_early_map((void*)kernel->kernel_bottom, kernel->kernel_bottom,
                kernel->kernel_top - kernel->kernel_bottom,
                MEM_ATTRIB_KERNEL);

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
    .type  = 0xFA
  };
  gdt[SEL_USER_DATA/8] = (gdt_entry_t) {
    .base  = 0x0,
    .limit = CS_BASE-1,
    .type  = 0xF2
  };
  gdt[SEL_ALL_DATA/8] = (gdt_entry_t) {
    .base  = 0x0,
    .limit = 0xffffffff,
    .type  = 0x92
  };

  // also setup the TSS.
  memset(&kernel_tss, 0, sizeof(kernel_tss));
  // TSS stack is always mapped at a fixed address.
  // it is used just to store the context data.
  kernel_tss.ss0  = SEL_KERNEL_DATA;
  kernel_tss.esp0 = THREAD_OFFSET + 2*PAGE_SIZE;
  gdt[SEL_TSS/8] = (gdt_entry_t) {
    .base = (uintptr_t)&kernel_tss,
    .limit = sizeof(kernel_tss),
    .type = 0x89
  };

  for (int i = 0; i < SEL_COUNT; ++i) {
    gdt_early_set_entry(i*8, gdt[i]);
  }

  // switch to segmented mode.
  gdt_early_enable_segments();
  tss_update();
}

void kernel_early_finalize(kernel_early_t *early) {
#ifdef DEBUG
  kprintf("kernel_early_finalize\n");
#endif
  
  // move the VGA text buffer.
  terminal_early_finalize();

  // remove write permissions from executable pages in the DS segment.
  mem_remap((void*)early->text_bottom, early->text_top - early->text_bottom,
            MEM_ATTRIB_PRESENT | MEM_ATTRIB_USERMODE);

  // protect stack my unmapping surrounding pages.
  mem_unmap_page(kernel_stack);
  mem_unmap_page(KERNEL_STACK_TOP);
  
  // initialize mem allocator with free pages.
  // TODO: unmap / free unneeded early kernel pages.
  // TODO: properly initialize the memory allocator.
  mem_early_finalize(early->memory_map, early->memory_map_elements,
                     early->kernel_bottom, early->kernel_top,
                     early->core_bottom, early->core_top);

  // map the core service image so that kernel can access it later.
  mem_map((void*)(CORE_OFFSET), early->core_bottom,
          early->core_top - early->core_bottom, MEM_ATTRIB_USER);
  mem_map((void*)(CS_BASE + CORE_OFFSET), early->core_bottom,
          early->core_top - early->core_bottom, MEM_ATTRIB_USER);
}

void kernel_early(uint32_t magic, multiboot_info_t *info) {
  mem_early_initialize();
  terminal_early_initialize();
  gdt_early_initialize();
  
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
