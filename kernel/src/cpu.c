#include "cpu.h"
#include "kernel.h"
#include "process.h"
#include "thread.h"
#include "page.h"
#include "lomem.h"
#include "log.h"
#include <hellbender/debug.h>

// small stack to run the kernel.
uint8_t cpu_stack[KERNEL_STACK_SIZE] __attribute__ ((aligned (PAGE_SIZE)));

// struct cpu cpu; is defined in cpu_asm.S to give it a funny address.

void cpu_enable_features() {
  // Ensure that CPUID.
  uint64_t id_bit;
  asm volatile ("pushfq                 \n"
                "pushfq                 \n"
                "xorq $0x200000, (%%rsp)\n"
                "popfq                  \n"
                "pushfq                 \n"
                "popq %0                \n"
                "xorq (%%rsp), %0       \n"
                "popfq                  \n"
                : "=r"(id_bit));
  if (!id_bit) log_error("cpu", "enable_features", "CPUID not supported");

  // CPU Vendor string.
  //uint32_t max_func, vendor1, vendor2, vendor3;
  //asm volatile ("cpuid" : "=a"(max_func), "=b"(vendor1), "=d"(vendor2), "=c"(vendor3));
  
  // Check support for execution-disable.
  register uint32_t func = 0x80000001;
  register uint32_t ebx, ecx, edx;
  asm volatile ("cpuid" : "+a"(func), "=b"(ebx), "=c"(ecx), "=d"(edx));
  if ((edx & (1<<20)) == 0) log_error("cpu", "enable_features", "No-execute not supported");

  // Enable no-execute feature:
  #define MSR_EFER 0xC0000080
  #define MSR_EFER_NXE (1<<11)
  asm volatile ("rdmsr               \n"
                "orq %0, %%rax       \n"
                "wrmsr               \n"
                : : "i"(MSR_EFER_NXE), "c"(MSR_EFER) : "rax", "rbx", "cc");
}

void cpu_init() {
  // allocate a page for CPU specific stuff.
  uintptr_t cpu_page = page_clear(lomem_alloc_4k());
  // put that page to the 510th entry in the PML4T.
  page_set_pdpt((void*)0xFFFFFF0000000000, cpu_page, PAGE_KERNEL_RW);
  // now the page can be accessed through recursive page mapping.
  // The 'cpu' symbol has the correct value to access the page.

  // initialize the structure.
  cpu.kernel_stack_top = cpu_stack + KERNEL_STACK_SIZE;

  // Finally we want to register this CPU to the kernel.
  kernel_add_cpu((struct cpu*)kernel_p2v(cpu_page));
}

void cpu_set_process(struct process* process) {
  if (cpu.current_process == process) {
    return;
  }
  cpu.current_process = process;
  if (process && process->usermode) {
    page_set_pdpt(0, kernel_v2p(process->pdpt), PAGE_USER_RW);
  } else {
    page_set_pdpt(0, 0, 0);
  }
  page_invalidate_all(); // TODO: use PCID to avoid invalidation.
}

void cpu_set_thread(struct thread* thread) {
  size_t invalidate_pages = 0; // how many thread local pages need to be invalidated.
  if (cpu.current_thread == thread) {
    return;
  }
  if (cpu.current_thread) {
    cpu.current_thread->rsp_backup = cpu.current_thread_rsp;
    invalidate_pages = cpu.current_thread->thread_local_pages;
  }
  cpu.current_thread = thread;
  if (thread) {
    cpu_set_process(thread->process);
    cpu.current_thread_rsp = thread->rsp_backup;
    cpu.tss.rsp_0 = (uint64_t)thread->stack_top;
    cpu.thread_local_pt = kernel_v2p(thread->thread_local_pt);
    if (thread->thread_local_pages > invalidate_pages) {
      invalidate_pages = thread->thread_local_pages;
    }
    // TODO: enable / disable user mode & service mode.
  } else {
    cpu.current_thread_rsp = 0;
    cpu.tss.rsp_0 = 0;
    cpu.thread_local_pt = 0;
  }

  thread_local_invalidate(invalidate_pages);
}
