#ifndef _EARLY_H
#define _EARLY_H

#include <stdint.h>
#include <kernel/multiboot.h>
#include <kernel/module.h>

// maximum number of memory segments from GRUB.
#define MAX_MEMORY_MAP_ELEMENTS 16

// maximum number of modules that can be loaded by GRUB (kernel included).
#define MAX_MODULES 4

typedef struct early_data {
  // memory map information copied from multiboot header.
  unsigned memory_map_elements;
  memory_map_t memory_map[MAX_MEMORY_MAP_ELEMENTS];

  // modules loaded by GRUB:
  module_binary_t binaries[MAX_MODULES]; // physical address range.
  kernel_module_t modules[MAX_MODULES]; // virtual address ranges.
} early_data_t;

/**
 * Initialization sequencec:
 *  - stage 1: process multiboot information.
 *  - stage 2: setup virtual memory.
 *  - stage 3: setup kernel components.
 *  - stage 4: setup core services.
 */
void early_state_1(uint32_t magic, multiboot_info_t *info);
void early_stage_2(early_data_t *data);
void early_stage_3(early_data_t *data);
void early_stage_4();
void early_stage_5();
  
#endif
