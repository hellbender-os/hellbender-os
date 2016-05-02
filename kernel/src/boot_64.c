#include "kernel.h"
#include "multiboot.h"
#include "vga.h"
#include "page.h"
#include "lomem.h"
#include "himem.h"
#include "cpu.h"
#include "gdt.h"
#include "idt.h"
#include "pic.h"
#include "service.h"

#include <string.h>

void boot_64(uint32_t magic, uint32_t multiboot) {
  // checking multiboot magic variable.
  VGA_AT(0,0) = VGA_ENTRY('h', WHITE_ON_BLACK);
  if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
    VGA_AT(0,0) = VGA_ENTRY('1', WHITE_ON_BLACK);
    kernel_panic();
  }

  // copying multiboot data.
  multiboot_info_t *info = (multiboot_info_t *)kernel_p2v((uintptr_t)multiboot);
  switch (multiboot_copy(info)) {
  case MULTIBOOT_TOO_MANY_MEMORY_MAPS:
    VGA_AT(0,0) = VGA_ENTRY('2', WHITE_ON_BLACK);
    kernel_panic();
  case MULTIBOOT_TOO_MANY_MODULES:
    VGA_AT(0,0) = VGA_ENTRY('3', WHITE_ON_BLACK);
    kernel_panic();
  case MULTIBOOT_TOO_MANY_ELF_HEADERS:
    VGA_AT(0,0) = VGA_ENTRY('4', WHITE_ON_BLACK);
    kernel_panic();
  }
  VGA_AT(0,0) = VGA_ENTRY('H', WHITE_ON_BLACK);

  // we no longer need the original memory map at zero address.
  VGA_AT(0,1) = VGA_ENTRY('e', WHITE_ON_BLACK);
  page_set_pdpt(0, 0, 0);
  page_invalidate_all();
  
  // initialize page allocators.
  lomem_init();
  himem_init();
  VGA_AT(0,1) = VGA_ENTRY('E', WHITE_ON_BLACK);

  // initialize CPUs.
  VGA_AT(0,2) = VGA_ENTRY('l', WHITE_ON_BLACK);
  cpu_enable_features();
  cpu_init();
  service_init();
  gdt_init(); // adds per-cpu TSS into GDT.
  tss_update();
  VGA_AT(0,2) = VGA_ENTRY('L', WHITE_ON_BLACK);

  // initialize interrupt handling.
  VGA_AT(0,3) = VGA_ENTRY('l', WHITE_ON_BLACK);
  pic_init();
  idt_init();
  VGA_AT(0,3) = VGA_ENTRY('L', WHITE_ON_BLACK);

  // kick-start the core service.
  VGA_AT(0,4) = VGA_ENTRY('b', WHITE_ON_BLACK);
  kernel_start_core();

  // start scheduler and do stuff.
  VGA_AT(0,4) = VGA_ENTRY('B', WHITE_ON_BLACK);
  kernel_main();
}
