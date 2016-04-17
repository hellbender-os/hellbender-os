#include "cpu.h"
#include "kernel.h"
#include "page.h"
#include "lomem.h"

// small stack to run the kernel.
uint8_t cpu_stack[CPU_STACK_SIZE] __attribute__ ((aligned (PAGE_SIZE)));

// struct cpu cpu; is defined in cpu_asm.S to give it a funny address.

void cpu_enable_features() {
  /*
Extended Feature Enable Register (EFER):
    The EFER is a model-specific register (MSR) with an address of C000_0080h
    No-Execute Enable (NXE) Bit. Bit 11. Setting this bit to 1 enables the no-execute page-protection feature.
    Before setting NXE, system software should verify the processor supports the feature by examining the extended feature flags returned by the CPUID instruction.
    Prior to using the CPUID instruction, you should also make sure the processor supports it by testing the 'ID' bit (0x200000) in eflags. This bit is modifiable only when the CPUID instruction is supported. For systems that don't support CPUID, changing the 'ID' bit will have no effect.
    You can check if the CPU supports the execute disable bit through CPUID/EAX=80000001h, bit 20 of EDX. 


Global-page extensions are controlled by setting and clearing
the PGE bit in CR4 (bit 7). When CR4.PGE is set to 1, globalpage
extensions are enabled. When CR4.PGE is cleared to 0,
global-page extensions are disabled. When CR4.PGE=1, setting
the global (G) bit in the translation-table entry marks the page
as global.
   */
}

void cpu_init() {
  // allocate a page for CPU specific stuff.
  uintptr_t cpu_page = page_clear(lomem_alloc_4k());
  // put that page to the 510th entry in the PML4T.
  page_set_pdpt((void*)0xFFFFFF0000000000, cpu_page, PAGE_KERNEL_RW);
  // now the page can be accessed through recursive page mapping.
  // The 'cpu' symbol has the correct value to access the page.

  // initialize the structure.
  cpu.kernel_stack_top = cpu_stack + CPU_STACK_SIZE;

  // Finally we want to register this CPU to the kernel.
  kernel_add_cpu((struct cpu*)kernel_p2v(cpu_page));
}

void cpu_set_current(struct thread* thread) {
  // it is important that we are not interrupted while swapping the thread.
  asm volatile ("cli");
  if (cpu.current_thread) {
    cpu.current_thread->rsp_backup = cpu.current_thread_rsp;
    cpu.current_thread = thread;
    cpu.current_thread_rsp = thread->rsp_backup;
    cpu.tss.rsp_0 = (uint64_t)thread->stack_top;
  }
  asm volatile ("sti");
}
