#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <kernel/domain.h>
#include <kernel/module.h>
#include <kernel/process.h>
#include <kernel/kernel.h>
#include <kernel/mem.h>
#include <kernel/mmap.h>
#include <kernel/vmem.h>
#include <kernel/elf32.h>

domain_t* domain_from_address(void* virtual) {
  uintptr_t addr = (uintptr_t)virtual;
  if (addr < KERNEL_LIMIT) return &kernel_domain;
  else if (addr < APPLICATION_LIMIT) return APPLICATION_DOMAIN;
  else return ((struct domain_first_page*)(addr & 0x7fc00000))->domain;
}

void* domain_set_break(domain_t *domain,
                       void* end_or_null,
                       intptr_t delta_or_zero) {
  if (end_or_null == NULL) end_or_null = domain->program_break + delta_or_zero;

  void *end = ceil_page(end_or_null);
  void *limit = domain->domain_base + domain->domain_size;
  if (end < domain->heap_bottom) return (void*)-1;
  if (end > limit) return (void*)-1;
  while (end < domain->program_break) {
    domain->program_break -= PAGE_SIZE;
    mem_free_page(mmap_unmap_page(domain->program_break));
  }
  if (end > domain->program_break) {
    size_t delta = end - domain->program_break;
    mem_alloc_mapped(domain->program_break, delta, MMAP_ATTRIB_USER_RW);
    domain->program_break += delta;
  }
  return domain->program_break;
}

void domain_update_data(domain_t *domain, void* address, size_t size) {
  void *top = ceil_page(address + size);
  if (top > domain->heap_bottom) domain->heap_bottom = top;
  if (top > domain->program_break) domain->program_break = top;
}

void domain_update_text(domain_t *domain, void* address, size_t size) {
  void *top = address + size;
  if (address < domain->text_bottom)  domain->text_bottom = address;
  if (top > domain->text_top) domain->text_top = top;
  domain_update_data(domain, address, size);
}

static domain_t* domain_create(void* base, size_t size) {
  domain_t *domain = malloc(sizeof(domain_t));
  memset(domain, 0, sizeof(domain_t));

  // allocate page tables.x
  domain->page_table_ds = mem_alloc_page_cleared();
  domain->page_table_cs = mem_alloc_page_cleared();

  // setup domain structure.
  domain->domain_base = base;
  domain->domain_size = size;
  domain->text_bottom = base;
  domain->text_top = base;
  domain->heap_bottom = base;
  domain->program_break = base;

  // setup the first page structure.
  domain->first_page = mem_alloc_page();
  struct domain_first_page *tmp = (struct domain_first_page*)
    mmap_temp_map(domain->first_page, MMAP_ATTRIB_KERNEL_RW);
  tmp->domain = domain;
  mmap_temp_unmap(tmp);
  
  uint32_t* tmp_ds =
    (uint32_t*)mmap_temp_map(domain->page_table_ds, MMAP_ATTRIB_KERNEL_RW);
  tmp_ds[0] = domain->first_page | MMAP_ATTRIB_USER_RO;
  mmap_temp_unmap(tmp_ds);

  uint32_t* tmp_cs =
    (uint32_t*)mmap_temp_map(domain->page_table_cs, MMAP_ATTRIB_KERNEL_RW);
  tmp_cs[0] = domain->first_page | MMAP_ATTRIB_USER_RO;
  mmap_temp_unmap(tmp_cs);

  return domain;
}

domain_t* domain_create_application() {
  domain_t *domain = domain_create((void*)APPLICATION_OFFSET, TABLE_SIZE);
  domain->heap_bottom += PAGE_SIZE; // first page reserved for domain info.
  domain->program_break += PAGE_SIZE;

  // copy the bootstrap code into the application domain.
  void* tmp = mmap_temp_map(domain->first_page, MMAP_ATTRIB_KERNEL_RW);
  void* bootstrap_code = (void*)kernel.core_service->process_bootstrap;
  memcpy(tmp + APPLICATION_BOOTSTRAP_START,
         bootstrap_code, APPLICATION_BOOTSTRAP_SIZE);
  mmap_temp_unmap(tmp);
  return domain;
}

/*
domain_t* domain_create_service() {
  domain_t *domain = domain_create();
  void *base = vmem_alloc_top_table();
  return domain_prepare(domain, base, TABLE_SIZE);
}
*/

domain_t kernel_domain;

domain_t* domain_create_kernel(kernel_module_t* module) {
  domain_t *domain = &kernel_domain;
  memset(domain, 0 , sizeof(domain_t));
  
  domain->domain_base = (void*)KERNEL_BASE;
  domain->domain_size = TABLE_SIZE;
  domain->text_bottom = (void*)module->text_bottom;
  domain->text_top = (void*)module->text_top;
  domain->heap_bottom = ceil_page((void*)module->top);
  domain->program_break = domain->heap_bottom;
  domain->start = (void*)module->start;
  return domain;
}

domain_t* domain_create_coresrv(kernel_module_t* module,
                                module_binary_t* binary) {
  void* base = (void*)CORE_OFFSET;
  if ((uintptr_t)base != module->bottom) {
    printf("core server module not loaded at expected address.\n");
    abort();
  }
  domain_t *domain = domain_create(base, TABLE_SIZE);

  // map module to its page table (we have to activate page table for this).
  domain_enable(domain);

  // move the loaded binary into its proper place at CORE_OFFSET.
  void* text_bottom = (void*)UINTPTR_MAX;
  void* text_top = 0;
  void* top = 0;
  Elf32_Ehdr *elf_header =
    (Elf32_Ehdr*)mmap_temp_map(binary->bottom, MMAP_ATTRIB_KERNEL_RO);
  for (unsigned i = 0; i < elf_header->e_phnum; ++i) {
    Elf32_Phdr *prog_header = (Elf32_Phdr*)
      ((uintptr_t)elf_header + elf_header->e_phoff
       + i * elf_header->e_phentsize);
    if (prog_header->p_type != PT_LOAD) continue;

    // virtual address range where pages will be moved into:
    void *vaddr = (void*)prog_header->p_vaddr;
    int vsize = (int)prog_header->p_memsz;
    void* vtop = vaddr + vsize;
    if (vtop > top) top = vtop;
    if (prog_header->p_flags & PF_X) {
      if (vaddr < text_bottom) text_bottom = vaddr;
      if (vtop > text_top) text_top = vtop;
    }

    // physical address range to move:
    uintptr_t paddr = binary->bottom + prog_header->p_offset;
    int psize = (int)prog_header->p_filesz;
    unsigned attrib = MMAP_ATTRIB_USER_RO;
    if (prog_header->p_flags & PF_W) attrib = MMAP_ATTRIB_USER_RW;
    
    // first move all pages that were in the file.
    while (psize > 0) {
      mmap_map_page(vaddr, paddr, attrib);
#if CS_BASE > 0
      if (prog_header->p_flags & PF_X) {
        mmap_map_page(CS_BASE + vaddr, paddr, MMAP_ATTRIB_USER_RO);
      }
#endif
      psize -= PAGE_SIZE;
      vsize -= PAGE_SIZE;
      paddr += PAGE_SIZE;
      vaddr += PAGE_SIZE;
    }
    // then allocate pages that were not in the file.
    while (vsize > 0) {
      uintptr_t page = mem_alloc_page();
      mmap_map_page(vaddr, page, MMAP_ATTRIB_USER_RW);
      memset(vaddr, 0, PAGE_SIZE);
      vsize -= PAGE_SIZE;
      vaddr += PAGE_SIZE;
    }
  }
  
  // quick check to see that core service struture is in place:
  core_service_t *core_service = (core_service_t*)elf_header->e_entry;
  if (core_service->this_size != sizeof(core_service_t)) {
    printf("Illegal core service header.\n");
    abort();
  }
  kernel.core_service = core_service;
  mmap_temp_unmap(elf_header);

  // write the domain address into the first_page (using the first IDC slot).
  struct domain_first_page *first_page = (struct domain_first_page*)CORE_OFFSET;
  first_page->domain = domain;
  
  // no need for the page table until some thread accesses it.
  domain_disable(domain);
  
  domain->text_bottom = text_bottom;
  domain->text_top = text_top;
  domain->heap_bottom = ceil_page(top);
  domain->program_break = domain->heap_bottom;
  domain->start = (void*)core_service->module_start;

  return domain;
}

void domain_enable(domain_t* domain) {
  if (domain->page_table_ds) {
    mmap_map_table(domain->domain_base, domain->page_table_ds,
                   MMAP_ATTRIB_USER_RW);
#if CS_BASE > 0
    mmap_map_table(CS_BASE + domain->domain_base, domain->page_table_cs,
                   MMAP_ATTRIB_USER_RO);
#endif
  }
}

void domain_disable(domain_t* domain) {
  if (domain->page_table_ds) {
    mmap_map_table(domain->domain_base, domain->page_table_ds,
                   MMAP_ATTRIB_KERNEL_RW);
#if CS_BASE > 0
    mmap_map_table(CS_BASE + domain->domain_base, domain->page_table_cs,
                   MMAP_ATTRIB_KERNEL_RO);
#endif
  }
}

void domain_push(uintptr_t entry_address, uintptr_t return_address) {
  if (CURRENT_THREAD->domain_idx >= IDC_CALL_MAX) {
    printf("Too many recursive IDC calls!\n");
    abort();
  }
  
  if (entry_address >= APPLICATION_LIMIT
      && entry_address < SERVICE_LIMIT
      && (entry_address & 0x3FF007) == 0
      && (entry_address & 0xFFF) != 0) {

    domain_t *domain = domain_from_address((void*)entry_address);
    CURRENT_THREAD->current_domain = domain;
    int found = 0;
    for (size_t i = 0; i < CURRENT_THREAD->domain_idx; ++i) {
      if (CURRENT_THREAD->domain_stack[i].domain == domain) {
        found = 1;
        break;
      }
    }
    if (!found) {
      domain_enable(domain);
    }
    
    struct domain_stack *stack =
      &CURRENT_THREAD->domain_stack[CURRENT_THREAD->domain_idx++];
    stack->domain = domain;
    stack->return_address = return_address;

  } else {
    printf("Illegal IDC address %x.\n", (unsigned)entry_address);
    abort();
  }

  CURRENT_THREAD->current_domain =
    CURRENT_THREAD->domain_stack[CURRENT_THREAD->domain_idx - 1].domain;
}

uintptr_t domain_pop() {
  if (CURRENT_THREAD->domain_idx == 1) {
    printf("Too many IDC pops!\n");
    abort();
  }
  struct domain_stack *stack =
    &CURRENT_THREAD->domain_stack[--CURRENT_THREAD->domain_idx];

  int found = 0;
  for (size_t i = 0; i < CURRENT_THREAD->domain_idx; ++i) {
    if (CURRENT_THREAD->domain_stack[i].domain == stack->domain) {
      found = 1;
      break;
    }
  }
  if (!found) {
    domain_disable(stack->domain);
  }
  
  CURRENT_THREAD->current_domain =
    CURRENT_THREAD->domain_stack[CURRENT_THREAD->domain_idx - 1].domain;
  return stack->return_address;  
}

void domain_restore_environment(domain_t *domain,
                                int *argc, int *envc, char **ptr) {
  *argc = domain->argc;
  *envc = domain->envc;
  void* next = *ptr = (char*)domain->program_break;
  
  // map all pages that were used when storing arguments and environment.
  uint32_t* pages =
    (uint32_t*)mmap_temp_map(domain->arg_table, MMAP_ATTRIB_KERNEL_RW);
  for (unsigned i = 0; i < PAGE_SIZE / sizeof(uint32_t); ++i) {
    if (pages[i]) {
      mmap_map_page(next, pages[i], MMAP_ATTRIB_USER_RW);
      next += PAGE_SIZE;
    } else break;
  }
  mmap_temp_unmap(pages);

  domain->program_break = next;
}

struct unmapped {
  uint32_t *tmp_table;
  char *tmp_data;
  unsigned n_pages;
  unsigned index;
};

static inline void init_unmapped(struct unmapped *um, uint32_t page_table) {
  um->tmp_table = (uint32_t*)mmap_temp_map(page_table, MMAP_ATTRIB_KERNEL_RW);
  um->n_pages = 1;
  um->index = 0;

  uint32_t page = mem_alloc_page();
  um->tmp_table[0] = page;
  um->tmp_data = mmap_temp_map(page, MMAP_ATTRIB_KERNEL_RW);;
}

static inline void tear_unmapped(struct unmapped *um) {
  mmap_temp_unmap(um->tmp_data);
  mmap_temp_unmap(um->tmp_table);
}

static inline void put_unmapped(struct unmapped *um, char c) {
  if (um->index == PAGE_SIZE) {
    if (um->n_pages == PAGE_SIZE/sizeof(uint32_t)) {
      printf("Too large environment.\n");
      abort();
    }
    uint32_t page = mem_alloc_page();
    um->tmp_table[um->n_pages++] = page;
    mmap_map_page(um->tmp_data, page, MMAP_ATTRIB_KERNEL_RW);
    um->index = 0;
  }
  um->tmp_data[um->index++] = c;
}

void domain_store_environment(domain_t *domain,
                              char *const* argv, char *const* envp) {
  // we copy data to pages that are not mapped in virtual memory.
  // thus, we collect all physical pages into a table.
  domain->arg_table = mem_alloc_page();
  struct unmapped um;
  init_unmapped(&um, domain->arg_table);

  // copy args
  int argc = 0;
  for (char *ptr = *(argv++); ptr; ptr = *(argv++), ++argc) {
    // copy next string, including terminator.
    do { put_unmapped(&um, *ptr); } while (*ptr++);
  }
  domain->argc = argc;

  // copy envp
  int envc = 0;
  for (char *ptr = *(envp++); ptr; ptr = *(envp++), ++envc) {
    // copy next string, including terminator.
    do { put_unmapped(&um, *ptr); } while (*ptr++);
  }
  domain->envc = envc;
}
