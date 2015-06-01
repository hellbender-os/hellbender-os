#ifndef _KERNEL_DOMAIN_H
#define _KERNEL_DOMAIN_H

#include <stddef.h>

#include <kernel/kernel.h>

typedef struct domain {
  void* domain_base; // beginning of the virtual address space.
  size_t domain_size; // virtual address space size; multiples of TABLE_SIZE.

  // TODO: support domains larger than TABLE_SIZE.
  // TODO: support domains with holes in their virtual memory.
  uintptr_t page_table_ds; // physical address of the page table.
  uintptr_t page_table_cs; // physical address of the page table.
  uintptr_t first_page; // the first page in DS and CS segments.
  uintptr_t arg_table; // address to the page containing arg data pages.

  void* text_bottom; // range that contains all code.
  void* text_top;
  void* heap_bottom; // pointer above any read only data.
  void* program_break; // pointer above the allocated memory.

  void* start; // address of the entry function.
  size_t argc; // number of arguments, environment variables.
  size_t envc;
} domain_t;

// this struct is in the first page of the domain address space (RO for users).
struct domain_first_page {
  domain_t *domain;
  // add any "public" domain info.
  // TODO: service IDC is also the first page, maybe this should be moved
  // to offset +1000, and all modules to +2000
};

#define APPLICATION_DOMAIN (((struct domain_first_page*)APPLICATION_OFFSET)->domain)
                            
extern domain_t kernel_domain;

domain_t* domain_from_address(void* virtual);

//domain_t* domain_create_service(); // allocated from the top of address space.
domain_t* domain_create_kernel(kernel_module_t* module);
domain_t* domain_create_coresrv(kernel_module_t* module,
                                module_binary_t* binary); // from loaded binary.
domain_t* domain_create_application(); // app bootstrap at fixed address.

void domain_update_data(domain_t *domain, void* address, size_t size);
void domain_update_text(domain_t *domain, void* address, size_t size);

void* domain_set_break(domain_t *domain,
                       void* end_or_null,
                       intptr_t delta_or_zero);

void domain_enable(domain_t* domain);
void domain_disable(domain_t* domain);

void domain_push(uintptr_t entry_address, uintptr_t return_address);
uintptr_t domain_pop();

void domain_store_environment(domain_t *domain,
                              char *const* argv, char *const* envp);
void domain_restore_environment(domain_t *domain,
                                int *argc, int *envc, char **ptr);

#endif
