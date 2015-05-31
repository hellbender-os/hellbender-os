#include "early.h"

#include <stdio.h>

#include <kernel/kernel.h>
#include <kernel/mem.h>
#include <kernel/mmap.h>
#include <kernel/tty.h>
#include <kernel/gdt.h>

/**
 * Now we have saved all the data we need from multiboot.
 *
 * - Set up paging with recursive page directory, and memory segments.
 * - Map the kernel memory into data and code segments.
 * - Map the VGA text buffer inside the kernel memory.
 *
 * We end up with a system where we have got rid of multiboot and are 
 * in a paged memory model with protected data and code segments. 
 *
 * Stage 3 will set up all kernel components.
 */
void early_stage_2(early_data_t *data) {
  //printf("early_stage_2\n");

  mem_stage_2_init();
  mmap_stage_2_init();
  
  // map kernel code into CS section.
  size_t kernel_size = (data->modules[MODULE_KERNEL].top
                        - data->modules[MODULE_KERNEL].bottom);
  size_t text_size = (data->modules[MODULE_KERNEL].text_top
                      - data->modules[MODULE_KERNEL].text_bottom);
#if CS_BASE > 0
  mmap_stage_2_map((void*)(CS_BASE + data->modules[MODULE_KERNEL].text_bottom),
                   data->modules[MODULE_KERNEL].text_bottom,
                   text_size, MMAP_ATTRIB_KERNEL_RO);
#endif
  // map the whole kernel into DS section for now, make code read only.
  mmap_stage_2_map((void*)data->modules[MODULE_KERNEL].bottom,
                   data->modules[MODULE_KERNEL].bottom,
                   kernel_size, MMAP_ATTRIB_KERNEL_RW);
  mmap_stage_2_map((void*)data->modules[MODULE_KERNEL].text_bottom,
                   data->modules[MODULE_KERNEL].text_bottom,
                   text_size, MMAP_ATTRIB_KERNEL_RO);
  // move the VGA buffer so that first 1MB is completely free.
  terminal_stage_2_remap();

  // enable paging & memory segments.
  mmap_stage_2_enable_paging();
  gdt_stage_2_init();
  mmap_stage_2_cleanup();
  
  early_stage_3(data);
}
