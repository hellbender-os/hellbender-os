#include "early.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>

#include <kernel/kernel.h>
#include <kernel/process.h>
#include <kernel/scheduler.h>
#include <kernel/pic.h>
#include <kernel/isr.h>
#include <kernel/pic_isr.h>
#include <kernel/vga.h>
#include <kernel/mmap.h>

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
  //printf("early_stage_4\n");
  
  early_data_t *data = (early_data_t*)kernel.early_data;
  core_service_t *service =
    (core_service_t*)data->modules[MODULE_CORE].module_info;

  // create core service process.
  unsigned core_idx = kernel.core_module = kernel.nof_processes++;
  kernel.processes[core_idx] =
    process_create_coresrv(&data->modules[MODULE_CORE],
                           &data->binaries[MODULE_CORE]);

  // map VGA memory to core service:
  mmap_map_page((void*)service->vga_buffer, (uintptr_t)VGA_MEMORY,
                MMAP_ATTRIB_USER_RW);

  // map initrd to core service:
  size_t initrd_size = (size_t)(data->binaries[MODULE_INITRD].top -
                                data->binaries[MODULE_INITRD].bottom);
  service->initrd_buffer = INITRD_OFFSET;
  service->initrd_size = initrd_size;
  mmap_map((void*)INITRD_OFFSET, data->binaries[MODULE_INITRD].bottom,
           initrd_size, MMAP_ATTRIB_USER_RO);

  // TODO: give only required permissions!
  asm volatile ("pushf;"
                "orl $0x3000, (%esp);"
                "popf;"
                );

  // core is ready to run.
  scheduler_add_thread(kernel.processes[core_idx]->thread);

  // wait until core service notifies that the pre-init is done.
  sem_t* to_core = sem_open("kernel_to_core", O_CREAT, (mode_t)0, (unsigned)0);
  sem_t* to_kernel = sem_open("core_to_kernel", O_CREAT, (mode_t)0, (unsigned)0);
  sem_wait(to_kernel);

  // setup hardware interrupts.
  isr_stage_4_pic();
  pic_isr_stage_4_init(service, kernel.processes[core_idx]);
  printf("Hardware interrupts activated.\n");

  // notify core service to continue; wait it till completes.
  sem_post(to_core);
  sem_wait(to_kernel);

  printf("Kernel initialization done.\n");

  /*TODO cleanup
  sem_close(to_core);
  sem_close(to_kernel);
  sem_unlink("kernel_to_core");
  sem_unlick("core_to_kernel");
  */
  
  // enter scheduling loop.
  free(kernel.early_data);
  kernel.early_data = NULL;
  kernel.up_and_running = 1;
  exit(0);
}
