#include "early.h"

#include <stdio.h>

#include <kernel/kernel.h>
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
  printf("early_stage_2\n");

  mmap_stage_2_init();
  // map kernel code into CS section.
  mmap_stage_2_map((void*)(CS_BASE + data->modules[0].text_bottom),
                   data->modules[0].text_bottom,
                   data->modules[0].text_top - data->modules[0].text_bottom,
                   MMAP_ATTRIB_KERNEL_RO);
  // map the whole kernel into DS section for now, make code read only.
  mmap_stage_2_map((void*)data->modules[0].bottom, data->modules[0].bottom,
                   data->modules[0].top - data->modules[0].bottom,
                   MMAP_ATTRIB_KERNEL_RW);
  mmap_stage_2_map((void*)data->modules[0].text_bottom,
                   data->modules[0].text_bottom,
                   data->modules[0].text_top - data->modules[0].text_bottom,
                   MMAP_ATTRIB_KERNEL_RO);
  // move the VGA buffer so that first 1MB is completely free.
  terminal_stage_2_remap();

  // enable paging & memory segments.
  mmap_stage_2_enable_paging();
  gdt_stage_2_init();
  mmap_stage_2_cleanup();
  
  early_stage_3(data);
}
