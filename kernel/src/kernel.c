#include "kernel.h"
#include "multiboot.h"
#include "process.h"
#include "lomem.h"
#include "page.h"
#include "elf32.h"

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
  if (kernel.n_cpus == KERNEL_MAX_CPUS) kernel_panic();
  kernel.cpus[kernel.n_cpus++] = cpu;
}

void kernel_start_core() {
  // create the core service.
  if (multiboot_data.coresrv_module == -1) kernel_panic();
  module_t *core_mod = multiboot_data.modules + multiboot_data.coresrv_module;
  Elf32_Ehdr *elf = (Elf32_Ehdr*)kernel_p2v(core_mod->mod_start);
  // each ELF header may end up as a memory block.
  // we also need one for initrd and stack each.
  struct process_descriptor* pd = process_alloc_descriptor(elf->e_phnum + 2);
  pd->entry_point = (uintptr_t)elf->e_entry;

  // create memory map description of the binary.
  uintptr_t vmem_top = 0;
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
    pm->v_base = (uintptr_t)prog_header->p_vaddr;
    pm->v_size = (size_t)prog_header->p_memsz;
    uintptr_t v_top = pm->v_base + pm->v_size;
    if (v_top > vmem_top) vmem_top = v_top;

    // actual memory address range to move:
    pm->m_base = kernel_p2v(core_mod->mod_start + prog_header->p_offset);
    pm->m_size = (size_t)prog_header->p_filesz;
  }

  // add a stack.
  uintptr_t stack = lomem_alloc_2M();
  size_t stack_size = TABLE_SIZE;
  if (!stack) kernel_panic();
  {
    struct process_memory *pm = pd->memory_maps + (pd->n_maps++);
    pm->flags = PAGE_WRITEABLE | PAGE_NOEXECUTE;
    pm->m_base = kernel_p2v(stack);
    pm->m_size = stack_size;
    pm->v_base = page_round_up(vmem_top);
    pm->v_size = stack_size;
    uintptr_t v_top = pm->v_base + pm->v_size;
    if (v_top > vmem_top) vmem_top = v_top;
    memset(pm->m_base, 0, stack_size);
    pd->stack_bottom = pm->v_base;
    pd->stack_top = pm->v_base + stack_size;
    // by convention, top-most stack element points to the bottom.
    uintptr_t *ptr = (uintptr_t*)kernel_p2v(stack + stack_size);
    ptr[-1] = pd->stack_bottom;
  }

  // add the initrd, if available.
  if (multiboot_data.initrd_module != -1) {
    module_t *initrd_mod = multiboot_data.modules + multiboot_data.coresrv_module;
    struct process_memory *pm = pd->memory_maps + (pd->n_maps++);
    pm->flags = PAGE_WRITEABLE | PAGE_NOEXECUTE;
    pm->m_base = kernel_p2v(initrd_mod->mod_start);
    pm->m_size = initrd_mod->mod_end - initrd_mod->mod_start;
    pm->v_base = page_round_up(vmem_top);
    pm->v_size = pm->m_size;
    uintptr_t v_top = pm->v_base + pm->v_size;
    if (v_top > vmem_top) vmem_top = v_top;

    // add initrd info as command line arguments to the bottom of the stack:
    char* ptr = (char*)kernel_p2v(stack);
    for (char *c = _ulltoa_n(pm->v_base, ptr, 9, 16); c > ptr; --c) c[-1] = '0';
    ptr += 9;
    for (char *c = _ulltoa_n(pm->v_size, ptr, 9, 16); c > ptr; --c) c[-1] = '0';
    ptr += 9;
    *(ptr++) = 0; // end of args.
    *(ptr++) = 0; // end of envs.
  }

  // make it happen!
  struct process *core = process_create(pd);
  cpu_set_current(core->threads[0]);
}

__attribute__((__noreturn__))
void kernel_main() {
  BREAK;
  asm volatile ( "jmp isr_to_usermode" );
  __builtin_unreachable();
}
