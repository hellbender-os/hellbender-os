#include "kernel.h"
#include "multiboot.h"
#include "vga.h"
#include "page.h"
#include "lomem.h"
#include "himem.h"
#include "cpu.h"

#include <string.h>

void boot_64(uint32_t magic, uint32_t multiboot) {
  // checking multiboot magic variable.
  VGA_AT(0,0) = VGA_ENTRY('H', WHITE_ON_BLACK);
  if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
    VGA_AT(0,1) = VGA_ENTRY('!', WHITE_ON_BLACK);
    kernel_panic();
  }

  // copying multiboot data.
  VGA_AT(0,1) = VGA_ENTRY('E', WHITE_ON_BLACK);
  multiboot_info_t *info = (multiboot_info_t *)kernel_p2v((uintptr_t)multiboot);
  switch (multiboot_copy(info)) {
  case MULTIBOOT_TOO_MANY_MEMORY_MAPS:
    VGA_AT(0,2) = VGA_ENTRY('?', WHITE_ON_BLACK);
    kernel_panic();
  case MULTIBOOT_TOO_MANY_MODULES:
    VGA_AT(0,2) = VGA_ENTRY('?', WHITE_ON_BLACK);
    kernel_panic();
  case MULTIBOOT_TOO_MANY_ELF_HEADERS:
    VGA_AT(0,2) = VGA_ENTRY('.', WHITE_ON_BLACK);
    kernel_panic();
  }

  // we no longer need the original memory map at zero address.
  page_set_pdpt(0, 0, 0);
  page_invalidate_all();
  
  // initialize page allocators.
  VGA_AT(0,2) = VGA_ENTRY('L', WHITE_ON_BLACK);
  lomem_init();
  himem_init();

  // initialize all CPUs.
  cpu_init();

  // start setting up kernel structures.
  VGA_AT(0,3) = VGA_ENTRY('L', WHITE_ON_BLACK);
  BREAK;
  kernel_main();
}
