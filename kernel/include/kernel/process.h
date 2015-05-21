#ifndef _KERNEL_PROCESS_H
#define _KERNEL_PROCESS_H

#include <kernel/domain.h>
#include <kernel/thread.h>
#include <kernel/module.h>

typedef struct process {
  domain_t *domain;
  thread_t *thread;
} process_t;

process_t *process_create_kernel(kernel_module_t *module,
                                 void* start_address);

// Does not add the module to scheduler!
process_t *process_create_module(kernel_module_t *module,
                                 module_binary_t* binary);

process_t *process_create_application(const char* path,
                                      char *const* argv,
                                      char *const* envp);

#endif
