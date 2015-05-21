#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

#include <kernel/process.h>
#include <kernel/thread.h>
#include <kernel/scheduler.h>
#include <kernel/mem.h>
#include <kernel/mmap.h>

process_t *process_create_kernel(kernel_module_t *module,
                                 void* start_address) {
  process_t *process = (process_t*)malloc(sizeof(process_t));
  process->domain = domain_create_kernel(module);
  process->thread = thread_create(process->domain, start_address);
  process->thread->is_kernel = 1;
  scheduler_add_thread(process->thread);
  return process;
}

process_t *process_create_module(kernel_module_t *module,
                                 module_binary_t* binary) {
  process_t *process = (process_t*)malloc(sizeof(process_t));
  process->domain = domain_create_module(module, binary);
  process->thread = thread_create(process->domain, (void*)module->start);
  return process;
}

// this physical page will be owned by the application, and will be replaced.
static uint8_t tmp_arg[PAGE_SIZE] __attribute__((aligned(PAGE_SIZE)));

process_t *process_create_application(const char* path,
                                      char *const* argv,
                                      char *const* envp) {
  process_t *process = (process_t*)malloc(sizeof(process_t));
  process->domain = domain_create_application();
  process->thread = thread_create(process->domain,
                                  process->domain->text_bottom);

  // store path.
  unsigned path_size = strlen(path) + 1;
  process->thread->exec_path = (char*)malloc(path_size);
  memcpy(process->thread->exec_path, path, path_size);

  // store argv, envp.
  unsigned int i = 0;
  const char *ptr;
  while ((ptr = *(argv++))) {
    unsigned len = strlen(ptr) + 1;
    if (i+len >= ARG_MAX) {
      printf("Too long argument list!\n");
      abort();
    }
    memcpy(tmp_arg + i, ptr, len);
    i += len;
  }
  tmp_arg[i++] = 0;
  process->thread->exec_envp = i;
  while ((ptr = *(envp++))) {
    unsigned len = strlen(ptr) + 1;
    if (i+len >= ARG_MAX) {
      printf("Too long environment list!\n");
      abort();
    }
    memcpy(tmp_arg + i, ptr, len);
    i += len;
  }
  tmp_arg[i++] = 0;

  // give the data page to the thread, replace it by a new one.
  process->thread->exec_args = mmap_unmap_page(tmp_arg);
  mmap_map_page(tmp_arg, mem_alloc_page(), MMAP_ATTRIB_KERNEL_RW);
  
  scheduler_add_thread(process->thread);
  return process;
}
