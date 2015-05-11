#include "early.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <shred.h>

#include <kernel/kernel.h>
#include <kernel/process.h>
#include <kernel/scheduler.h>
#include <kernel/semaphore.h>
#include <kernel/pic.h>
#include <kernel/isr.h>
#include <kernel/pic_isr.h>

/**
 * The whole kernel is now on-line, and we are in multiprocessing
 * environment. We just have to setup core services to provide basic
 * OS functionality, and we are good to go. 
 *
 * - Setup processing contexts for the core services.
 * - Wait for core service to run pre-initialization.
 * - Setup hardware interrupts; notify core initializer.
 * - Wait for core services to run post-initialization.
 *
 * Kernel and core OS services are now up and running!
 * Final stage runs an application to test IPC system.
 */
void early_stage_4() {
  printf("early_stage_4\n");
  early_data_t *data = (early_data_t*)kernel.early_data;
    
  // create core service process.
  unsigned core_idx = kernel.core_module;
  kernel.processes[core_idx] =
    process_create_module(&data->modules[core_idx],
                          &data->binaries[core_idx]);
  scheduler_add_thread(kernel.processes[core_idx]->thread);

  // wait until core service notifies that the pre-init is done.
  semaphore_t* to_core = semaphore_create("kernel_to_core");
  semaphore_t* to_kernel = semaphore_create("core_to_kernel");
  semaphore_wait(to_kernel);

  printf("Kernel setting hardware interrupts.\n");
  // setup hardware interrupts.
  //pic_isr_stage_4_init();
  //isr_stage_4_pic();
  printf("Hardware interrupts activated.\n");

  // notify core service to continue; wait it till completes.
  semaphore_post(to_core);
  semaphore_wait(to_kernel);

  printf("Kernel initialization done.\n");

  /*TODO cleanup
  sem_close(to_core);
  sem_close(to_kernel);
  sem_unlink("kernel_to_core");
  sem_unlick("core_to_kernel");
  */
  
  early_stage_5();
}
