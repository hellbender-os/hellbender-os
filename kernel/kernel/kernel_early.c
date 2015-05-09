#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <kernel/kernel.h>
#include <kernel/kstdlib.h>
#include <kernel/kstdio.h>
#include <kernel/tty.h>
#include <kernel/mem.h>
#include <kernel/mmap.h>
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
  // physical address ranges where kernel modules are loaded.
  // the first one is the actual kernel.
  unsigned nof_binaries;
  module_binary_t binaries[MAX_MODULES];
  
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

void kernel_early_process_info(kernel_early_t *early,
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
      if (early->memory_map_elements > MAX_MEMORY_MAP_ELEMENTS) {
        kprintf("Maximum number of memory map elements exceeded!");
        kabort();
      }
      memcpy(&early->memory_map[early->memory_map_elements++],
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
  {
    kprintf("Kernel found at %x - %x; code at %x - %x\n",
            (unsigned)min_data, (unsigned)max_data,
            (unsigned)min_text, (unsigned)max_text);
    module_binary_t binary;
    binary.bottom = min_data;
    binary.top = max_data;
    kernel_module_t module = {0};  
    module.bottom = min_data;
    module.top = max_data;
    module.text_bottom = min_text;
    module.text_top = max_text;
    // we put kernel and module data into these arrays.
    early->binaries[0] = binary;
    early->nof_binaries = 1;
    kernel.modules[0] = module;
    kernel.nof_modules = 1;
  }
  
  // find all modules, make sure core is one of them.
  int core_found = 0;
  module_t *modules = (module_t*)info->mods_addr;
  for (unsigned i = 0; i < info->mods_count; ++i) {
    module_binary_t binary;
    binary.bottom = (uintptr_t)modules[i].mod_start;
    binary.top = (uintptr_t)modules[i].mod_end;
    kernel_module_t *mod_ptr = (kernel_module_t*)binary.bottom;
    int idx = early->nof_binaries++;
    early->binaries[idx] = binary;
    kernel.modules[idx] = *mod_ptr;
    kernel.nof_modules = early->nof_binaries;

    if (mod_ptr->bottom == CORE_OFFSET) {
      core_found = 1;
      kprintf("Found core service at %x - %x\n",
              binary.bottom, binary.top);
    } else {
      kprintf("Found a module at %x - %x\n",
              binary.bottom, binary.top);
    }
    kprintf("Mapped into %x - %x\n", mod_ptr->bottom, mod_ptr->top);
  }
  if (!core_found) {
    kprintf("Core service not found!\n");
    kabort();
  }
}

void kernel_early_init_paging() {
#ifdef DEBUG
  kprintf("kernel_early_init_paging\n");
#endif
  
  // map kernel code into CS section.
  mmap_early_map((void*)(CS_BASE + kernel.modules[0].text_bottom),
                 kernel.modules[0].text_bottom,
                 kernel.modules[0].text_top - kernel.modules[0].text_bottom,
                 MMAP_ATTRIB_KERNEL);
  
  // map the whole kernel into DS section for now, we'll remove code later.
  mmap_early_map((void*)kernel.modules[0].bottom, kernel.modules[0].bottom,
                 kernel.modules[0].top - kernel.modules[0].bottom,
                 MMAP_ATTRIB_KERNEL);

  // and enable paging.
  mmap_early_enable_paging();
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

  // prepare the pages tables for actual use.
  mmap_early_finalize();

  mem_early_initialize(early->memory_map, early->memory_map_elements,
                       early->binaries, early->nof_binaries);
  
  // NOW WE CAN USE NON-EARLY FUNCTIONS!
  
  // move the VGA text buffer to free up the virtual address space under 1MB.
  terminal_finalize();

  // protect stack by unmapping surrounding pages,
  // and make those guard pages available for use.
  mem_free_page(mmap_unmap_page(kernel_stack));
  mem_free_page(mmap_unmap_page(KERNEL_STACK_TOP));

  // map modules so that kernel can access them later.
  for (unsigned i = 1; i < early->nof_binaries; ++i) {
    mmap_map((void*)(kernel.modules[i].bottom), early->binaries[i].bottom,
             kernel.modules[i].top - kernel.modules[i].bottom,
             MMAP_ATTRIB_USER);
    unsigned text_offset =
      kernel.modules[i].text_bottom - kernel.modules[i].bottom;
    unsigned text_size =
      kernel.modules[i].text_top - kernel.modules[i].text_bottom;
    mmap_map((void*)(CS_BASE + kernel.modules[i].text_bottom),
             early->binaries[i].bottom + text_offset,
             text_size, MMAP_ATTRIB_PRESENT|MMAP_ATTRIB_USERMODE);
  }

  // remove write permissions from executable pages in the DS segment.
  mmap_remap((void*)kernel.modules[0].text_bottom,
             kernel.modules[0].text_top - kernel.modules[0].text_bottom,
             MMAP_ATTRIB_PRESENT);
  for (unsigned i = 0; i < early->nof_binaries; ++i) {
    mmap_remap((void*)kernel.modules[i].text_bottom,
               kernel.modules[i].text_top - kernel.modules[i].text_bottom,
               MMAP_ATTRIB_PRESENT | MMAP_ATTRIB_USERMODE);
  }
}

void kernel_early(uint32_t magic, multiboot_info_t *info) {
  mmap_early_initialize();
  terminal_early_initialize();
  gdt_early_initialize();
  
#ifdef DEBUG
    kprintf("kernel_early\n");
#endif
    
  if (magic == MULTIBOOT_BOOTLOADER_MAGIC) {
    kernel_early_t early = {0};
    kernel_early_process_info(&early, info);
    kernel_early_init_paging();
    kernel_early_init_segments(&early);
    kernel_early_finalize(&early);
  } else {
    kprintf("No multiboot!");
    kabort();
  }
#ifdef DEBUG
  kprintf("kernel_early DONE!\n");
#endif
}
