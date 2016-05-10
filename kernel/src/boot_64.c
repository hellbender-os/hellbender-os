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
  if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
    kernel_panic();
  }

  // copying multiboot data.
  multiboot_info_t *info = (multiboot_info_t *)kernel_p2v((uintptr_t)multiboot);
  switch (multiboot_copy(info)) {
  case MULTIBOOT_TOO_MANY_MEMORY_MAPS:
    kernel_panic();
  case MULTIBOOT_TOO_MANY_MODULES:
    kernel_panic();
  case MULTIBOOT_TOO_MANY_ELF_HEADERS:
    kernel_panic();
  }

  // we no longer need the original memory map at zero address.
  page_set_pdpt(0, 0, 0);
  page_invalidate_all();
  
  // initialize page allocators.
  lomem_init();
  himem_init();

  // setup text console emulation so that we can output debug stuff.
  vga_init();
  vga_putc('H');

  // initialize CPUs.
  cpu_enable_features();
  vga_putc('E');
  cpu_init();
  vga_putc('L');
  service_init();
  vga_putc('L');
  gdt_init(); // adds per-cpu TSS into GDT.
  vga_putc('B');
  tss_update();
  vga_putc('E');

  // initialize interrupt handling.
  pic_init();
  vga_putc('N');
  idt_init();
  vga_putc('D');

  // kick-start the core service.
  kernel_start_core();
  vga_putc('E');

  // start scheduler and do stuff.
  vga_putc('R');
  vga_newl();
  kernel_main();
}
