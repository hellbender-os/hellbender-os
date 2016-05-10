#include "kernel.h"
#include "multiboot.h"
#include "process.h"
#include "lomem.h"
#include "page.h"
#include "elf32.h"
#include "vga.h"
#include "scheduler.h"
#include "service.h"
#include "log.h"

#include <hellbender/debug.h>
#include <hellbender/libc_init.h>
#include <hellbender/coresrv_init.h>

#include <stddef.h>
#include <stdlib.h>

// this is used by some inline assembler to ensure memory ordering.
unsigned long __force_order;

struct kernel kernel;

__attribute__((__noreturn__))
void kernel_panic() {
  // TODO: Add proper kernel panic.
  asm("cli;1:hlt;jmp 1b");
  __builtin_unreachable();
}

void kernel_add_cpu(struct cpu* cpu) {
  SPIN_GUARD_RAW(kernel.lock);
  if (kernel.n_cpus == KERNEL_MAX_CPUS) {
    log_warn("kernel", "add_cpu", "Too many CPUs");
  } else {
    kernel.cpus[kernel.n_cpus++] = cpu;
  }
}

void kernel_start_core() {
  // create the core service.
  if (multiboot_data.coresrv_module == -1) {
    log_error("kernel", "start_core", "Coresrv module not found");
  }
  module_t *core_mod = multiboot_data.modules + multiboot_data.coresrv_module;
  Elf32_Ehdr *elf = (Elf32_Ehdr*)kernel_p2v(core_mod->mod_start);
  // each ELF header may end up as a memory block.
  // we also need one for initrd, stack, and VGA each.
  struct process_descriptor* pd = process_alloc_descriptor(elf->e_phnum + 3);
  pd->vmem_base = 0;
  pd->vmem_size = USERMODE_SIZE;
  service_relocate(pd);
  pd->entry_point = pd->vmem_base + (uintptr_t)elf->e_entry;

  // create memory map description of the binary.
  uintptr_t vmem_top = 0;
  struct process_memory *pm_local = 0; // thread local section.
  for (unsigned i = 0; i < elf->e_phnum; ++i) {
    Elf32_Phdr *prog_header = (Elf32_Phdr*)
      ((uintptr_t)elf + elf->e_phoff + i * elf->e_phentsize);
    if (prog_header->p_type != PT_LOAD) continue;
    struct process_memory *pm = pd->memory_maps + (pd->n_maps++);

    // check flags:
    pm->flags = 0;
    if ((prog_header->p_flags & PF_X) == 0) pm->flags |= PAGE_NOEXECUTE;
    if ((prog_header->p_flags & PF_W) != 0) pm->flags |= PAGE_WRITEABLE;

    // virtual address range where pages will be moved into:
    pm->v_base = pd->vmem_base + (uintptr_t)prog_header->p_vaddr;
    pm->v_size = (size_t)prog_header->p_memsz;
    if (pm->v_base % TABLE_SIZE) { // each block must be at table boundary!
      log_error("kernel", "start_core", "Unaligned ELF segment");
    }

    // actual memory address range to move:
    pm->m_base = kernel_p2v(core_mod->mod_start + prog_header->p_offset);
    pm->m_size = (size_t)prog_header->p_filesz;

    // thread local section requires special care:
    if (pm->v_base == pd->vmem_base + THREAD_LOCAL_BASE) {
      pm->flags = 0;
      pm_local = pm;
    } else {
      uintptr_t v_top = pm->v_base + pm->v_size;
      if (v_top > vmem_top) vmem_top = v_top;
    }
  }

  // add a stack.
  uintptr_t stack = page_clear(lomem_alloc_4k());
  size_t stack_size = PAGE_SIZE;
  uintptr_t stack_top = stack + stack_size;
  if (!stack) log_error("kernel", "start_core", "Out of memory");
  {
    struct process_memory *pm = pd->memory_maps + (pd->n_maps++);
    pm->flags = PAGE_WRITEABLE | PAGE_NOEXECUTE;
    pm->m_base = kernel_p2v(stack);
    pm->m_size = -(intptr_t)stack_size;
    pm->v_base = page_table_round_up(vmem_top);
    pm->v_size = USER_STACK_SIZE;
    uintptr_t v_top = pm->v_base + pm->v_size;
    if (v_top > vmem_top) vmem_top = v_top;
    pd->stack_bottom = pm->v_base;
    pd->stack_top = pm->v_base + pm->v_size;
  }

  // top of stack is used for libc & coresrv initialization data.
  pd->stack_reserved = sizeof(struct libc_init) + sizeof(struct coresrv_init);
  if (pd->stack_reserved > stack_size) {
    log_error("kernel", "start_core", "Stack too small");
  }
  struct coresrv_init *coresrv = 
    (struct coresrv_init *)kernel_p2v(stack_top - sizeof(struct coresrv_init));
  struct libc_init *libc = (struct libc_init *)kernel_p2v(stack_top - pd->stack_reserved);
  libc->data = (void*)(pd->stack_top - sizeof(struct coresrv_init));
  pd->libc = libc;

  // special thread locals init memory.
  if (pm_local) {
    pm_local->v_base = page_table_round_up(vmem_top);
    uintptr_t v_top = pm_local->v_base + pm_local->v_size;
    if (v_top > vmem_top) vmem_top = v_top;
    libc->threadlocal_init = (void*)pm_local->v_base;
    libc->threadlocal_size = pm_local->v_size;
  }

  // add VGA.
  {
    struct process_memory *pm = pd->memory_maps + (pd->n_maps++);
    pm->flags = PAGE_WRITETHROUGH | PAGE_WRITEABLE | PAGE_NOEXECUTE;
    pm->m_base = vga.fb;
    pm->m_size = vga.fb_size;
    pm->v_base = page_table_round_up(vmem_top);
    pm->v_size = pm->m_size;
    uintptr_t v_top = pm->v_base + pm->v_size;
    if (v_top > vmem_top) vmem_top = v_top;
    coresrv->vga_base = (void*)pm->v_base;
    coresrv->vga_size = pm->v_size;
  }

  // add the initrd, if available.
  if (multiboot_data.initrd_module != -1) {
    module_t *initrd_mod = multiboot_data.modules + multiboot_data.initrd_module;
    struct process_memory *pm = pd->memory_maps + (pd->n_maps++);
    pm->flags = PAGE_WRITEABLE | PAGE_NOEXECUTE;
    pm->m_base = kernel_p2v(initrd_mod->mod_start);
    pm->m_size = initrd_mod->mod_end - initrd_mod->mod_start;
    pm->v_base = page_table_round_up(vmem_top);
    pm->v_size = pm->m_size;
    uintptr_t v_top = pm->v_base + pm->v_size;
    if (v_top > vmem_top) vmem_top = v_top;
    coresrv->initrd_base = (void*)pm->v_base;
    coresrv->initrd_size = pm->v_size;
  }

  // make it happen!
  struct process *core = process_create(pd);
  (void)core;
}

__attribute__((__noreturn__))
void kernel_main() {
  // enter user mode the first time somewhat manually:
  scheduler_run(0);
  --cpu.nested_isr;
  asm volatile ( "jmp isr_to_usermode" );
  __builtin_unreachable();
}
