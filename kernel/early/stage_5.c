#include "early.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <kernel/kernel.h>
#include <kernel/process.h>
#include <kernel/scheduler.h>

/**
 * This will just launch an application that is loaded by the GRUB.
 */
void early_stage_5() {
  printf("early_stage_5\n");
  early_data_t *data = (early_data_t*)kernel.early_data;
    
  // create application process.
  if (data->binaries[MODULE_TEST].bottom) {
    unsigned app_idx = kernel.nof_processes++;
    kernel.processes[app_idx] =
    process_create_module(&data->modules[MODULE_TEST],
                          &data->binaries[MODULE_TEST]);
    scheduler_add_thread(kernel.processes[app_idx]->thread);
  }
  
  // enter main kernel loop.
  free(kernel.early_data);
  kernel.early_data = NULL;
  kernel_main();


/*
static int compare_tokens(const void* first, const void* second) {
  if (((uintptr_t*)first) == ((uintptr_t*)second)) return 0;
  return ((uintptr_t*)first) > ((uintptr_t*)second) ? -1 : 1;
}

void kernel_init_modules() {
  // token is module (page aligned) address + index into the modules array.
  // one byte is enough for index for MAX_MODULES is very small.
  // sorting such tokens allows us to process modules array in correct order.

  uintptr_t tokens[MAX_MODULES];
  for (unsigned i = 1; i < kernel.nof_modules; ++i) { // skip kernel "module".
    tokens[i - 1] = kernel.modules[i].bottom | (i & 0xff);
  }
  qsort(tokens, kernel.nof_modules - 1, sizeof(uintptr_t), compare_tokens);
  
  // process for each module, init by ascending address.
  for (unsigned j = 0; j < kernel.nof_modules - 1; ++j) {
    unsigned idx = tokens[j] & 0xff;
    printf("init mod: %u\n", idx);
    process_t *process = process_create_module(&kernel.modules[idx],
                                               &kernel.binaries[idx]);
    scheduler_add_thread(process->thread);
    kernel.processes[idx] = process;
  }
}
  */

  
}
