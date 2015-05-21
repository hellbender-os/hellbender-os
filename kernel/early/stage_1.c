#include "early.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <kernel/multiboot.h>
#include <kernel/elf32.h>
#include <kernel/kernel.h>
#include <kernel/tty.h>
#include <kernel/idt.h>
#include <kernel/isr.h>

/**
 * Stage 1 of kernel initialization begins with the following:
 *
 * - Multiboot has set up protected mode with A20 gate enabled.
 * - Multiboot has loaded kernel to the address specified at link time.
 * - Multiboot has loaded modules to their respective link time addresses.
 * - Multiboot has scanend for available memory and placed this info in memory.
 * - boot.S has set up our stack.
 *
 * We can ONLY use memory area that have been statically allocated,
 * otherwise we risk overwriting multiboot related data. What we do first?
 *
 * - Setup text console so that we can write debugging information.
 * - Set up boot time interrupt handlers to catch traps.
 * - Copy all multiboot information into a stack allocated structure.
 *
 * For now, any interrupt will cause basic debugging information to be displayed.
 * Stage 2 will continue by setting up virtual memory.
 */
void early_stage_1(uint32_t magic, multiboot_info_t *info) {
  // first things first: terminal console to print to.
  terminal_stage_1_init();
  //printf("early_stage_1\n");

  // setup debugging interrupt handlers
  idt_stage_1_init();
  isr_stage_1_debugger();
  idt_load();

  if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
    printf("No multiboot!");
    abort();
  }

  early_data_t data;
  memset(&data, 0, sizeof(data));
  memset(&kernel, 0, sizeof(kernel));

  // copy all available memory map elements.
  uintptr_t mm_end = info->mmap_addr + info->mmap_length;
  for (uintptr_t mm = info->mmap_addr; mm < mm_end; mm += sizeof(uint32_t)) {
    memory_map_t *map = (memory_map_t*)mm;
    /*
      printf("Memory .type=%u; .base=%u; .size=%u\n",
      (unsigned)map->type,
      (unsigned)map->base_addr_low,
      (unsigned)map->length_low);
    */
    if (map->type == 1) {
      if (data.memory_map_elements > MAX_MEMORY_MAP_ELEMENTS) {
        printf("Maximum number of memory map elements exceeded!");
        abort();
      }
      memcpy(&data.memory_map[data.memory_map_elements++],
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
      min_data = min_data < shdr[i].sh_addr ? min_data : shdr[i].sh_addr;
      uint32_t sh_top = shdr[i].sh_addr + shdr[i].sh_size;
      max_data = max_data > sh_top ? max_data : sh_top;
      if (shdr[i].sh_flags & SHF_EXECINSTR) {
        min_text = min_text < shdr[i].sh_addr ? min_text : shdr[i].sh_addr;
        max_text = max_text > sh_top ? max_text : sh_top;
      }
      //const char *name = &((const char *)shdr[i].sh_name)[shstrtab];
      //printf("Section %s at %u; %u bytes; type = %u\n",
      //     name,
      //     (unsigned)shdr[i].sh_addr,
      //     (unsigned)shdr[i].sh_size,
      //     (unsigned)shdr[i].sh_type);
    }
  }
  if (min_text < KERNEL_OFFSET) {
    printf("Kernel loaded too low!");
    abort();
  }

  // these are the limits for the whole kernel, and kernel code.
  {
    printf("Kernel  found at %x - %x; code at %x - %x\n",
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
    data.binaries[MODULE_KERNEL] = binary;
    data.modules[MODULE_KERNEL] = module;
  }
  
  // find all modules, make sure core is one of them.
  int core_found = 0;
  int initrd_found = 0;
  module_t *modules = (module_t*)info->mods_addr;
  for (unsigned i = 0; i < info->mods_count; ++i) {
    module_binary_t binary;
    binary.bottom = (uintptr_t)modules[i].mod_start;
    binary.top = (uintptr_t)modules[i].mod_end;

    if (strcmp((char*)modules[i].string, "--core") == 0) {
      // core has IDC table in the beginning.
      kernel_module_t *mod_ptr = (kernel_module_t*)(binary.bottom+4096);
      if (mod_ptr->magic != 0x1337c0de) {
        printf("No magic in core!\n");
        abort();
      }
      core_found = 1;
      data.binaries[MODULE_CORE] = binary;
      data.modules[MODULE_CORE] = *mod_ptr;
      printf("Coresrv found at %x - %x; ",
             (unsigned)binary.bottom, (unsigned)binary.top);
      printf("mapped into %x - %x\n",
             (unsigned)mod_ptr->bottom, (unsigned)mod_ptr->top);
    }
    else if (strcmp((char*)modules[i].string, "--test") == 0) {
      // module header at zero.
      kernel_module_t *mod_ptr = (kernel_module_t*)(binary.bottom);
      if (mod_ptr->magic != 0x1337c0de) {
        printf("No magic in core!\n");
        abort();
      }
      data.binaries[MODULE_TEST] = binary;
      data.modules[MODULE_TEST] = *mod_ptr;
      printf("Testapp found at %x - %x; ",
              (unsigned)binary.bottom, (unsigned)binary.top);
      printf("mapped into %x - %x\n",
             (unsigned)mod_ptr->bottom, (unsigned)mod_ptr->top);
    }
    else if (strcmp((char*)modules[i].string, "--initrd") == 0) {
      initrd_found = 1;
      data.binaries[MODULE_INITRD] = binary;
      unsigned initrd_size = (unsigned)(binary.top - binary.bottom);
      printf("Initrd  found at %x - %x; ",
              (unsigned)binary.bottom, (unsigned)binary.top);
      printf("mapped into %x - %x\n",
             (unsigned)INITRD_OFFSET, (unsigned)INITRD_OFFSET + initrd_size);
    }
    else {
      printf("Unknown found at %x - %x; \n",
              (unsigned)binary.bottom, (unsigned)binary.top);
    }
  }

  if (!core_found) {
    printf("Core service not found!\n");
    abort();
  }
  if (!initrd_found) {
    printf("Initrd not found!\n");
    abort();
  }

  early_stage_2(&data);
}
