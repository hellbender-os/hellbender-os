#include <stdlib.h>
#include <stdio.h>

#include <kernel/process.h>
#include <kernel/thread.h>

process_t *process_create_kernel(kernel_module_t *module,
                                 void* start_address) {
  process_t *process = (process_t*)malloc(sizeof(process_t));
  process->domain = domain_create_kernel(module);
  process->thread = thread_create(process->domain, start_address);
  process->thread->is_kernel = 1;
  return process;
}

process_t *process_create_module(kernel_module_t *module,
                                 module_binary_t* binary) {
  process_t *process = (process_t*)malloc(sizeof(process_t));
  process->domain = domain_create_module(module, binary);
  process->thread = thread_create(process->domain, (void*)module->start);
  return process;
}
