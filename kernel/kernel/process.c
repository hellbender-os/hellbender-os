#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

#include <kernel/process.h>
#include <kernel/thread.h>
#include <kernel/scheduler.h>
#include <kernel/mem.h>
#include <kernel/mmap.h>

process_t *process_create_kernel(domain_t *kernel_domain,
                                 void* start_address) {
  process_t *process = (process_t*)malloc(sizeof(process_t));
  memset(process, 0, sizeof(process_t));
  process->domain = kernel_domain;
  process->thread = thread_create(process->domain, start_address);
  process->thread->is_kernel = 1;
  scheduler_add_thread(process->thread);
  return process;
}

process_t *process_create_coresrv(kernel_module_t *module,
                                  module_binary_t* binary) {
  process_t *process = (process_t*)malloc(sizeof(process_t));
  process->domain = domain_create_coresrv(module, binary);
  process->thread = thread_create(process->domain, process->domain->start);
  return process;
}

process_t *process_create_application(const char* path) {
  process_t *process = (process_t*)malloc(sizeof(process_t));
  process->domain = domain_create_application();
  process->thread =
    thread_create(process->domain,
                  process->domain->domain_base + APPLICATION_BOOTSTRAP_START);

  // store path.
  unsigned path_size = strlen(path) + 1;
  process->thread->exec_path = (char*)malloc(path_size);
  memcpy(process->thread->exec_path, path, path_size);
  
  scheduler_add_thread(process->thread);
  return process;
}
