#include "cpu.h"
#include "kernel.h"
#include "page.h"
#include "lomem.h"

// small stack to run the kernel.
uint8_t cpu_stack[CPU_STACK_SIZE] __attribute__ ((aligned (PAGE_SIZE)));

// struct cpu cpu; is defined in cpu_asm.S to give it a funny address.

void cpu_init() {
  // allocate a page for CPU specific stuff.
  uintptr_t cpu_page = page_clear(lomem_alloc_4k());
  // put that page to the 510th entry in the PML4T.
  page_set_pdpt((void*)0xFFFFFF0000000000, cpu_page, PAGE_KERNEL_RW);
  // now the page can be accessed through recursive page mapping.
  // The 'cpu' symbol has the correct value to access the page.
  
  // Finally we want to register this CPU to the kernel.
  kernel_add_cpu((struct cpu*)kernel_p2v(cpu_page));
}

