#include "early.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <kernel/kernel.h>
#include <kernel/mmap.h>
#include <kernel/mem.h>
#include <kernel/vmem.h>
#include <kernel/tty.h>
#include <kernel/process.h>
#include <kernel/scheduler.h>
#include <kernel/isr.h>
#include <kernel/pic.h>

extern void _hellbender_libc_init();
extern void _init();

/**
 * Virtual memory has been setup, and we are ready to initialize kernel.
 *
 * - Initialize physical page allocator so that we can start using the memory.
 * - Initialize virtual memory allocator so that domains can be created.
 * - Setup processing context for kernel thread.
 * - Initialize the standard library.
 * - Call kernel global constructors.
 * - Set proper interrupt processing routines.
 * - Initialize scheduler.
 * - Continue initialization in multithreading context using scheduler.
 *
 * Now we have memory management, libK, interrupts, and scheduling.
 * That's pretty much the whole kernel. Time to initialize core services
 * in user space.
 */
void early_stage_3(early_data_t *data) {
  printf("early_stage_3\n");
  // setup physical page allocator, and virtual memory allocator.
  mem_stage_3_init(data->memory_map, data->memory_map_elements,
                   data->binaries, data->nof_modules);
  vmem_stage_3_init();

  // this will release the memory that is covered by the VGA buffer.
  terminal_stage_3_cleanup();
  
  // C language initialization.
  // TODO: kernel.heal_* not needed once 'set program break' is used.
  kernel.heap_bottom = ceil_page((void*)data->modules[0].top);
  kernel.heap_limit = (void*)KERNEL_HEAP_TOP;
  _hellbender_libc_init();
  _init();

  // kernel thread execution context.
  kernel.processes[0] = process_create_kernel(&data->modules[0],
                                              &early_stage_4);
  kernel.nof_processes = 1;
  thread_set_current(kernel.processes[0]->thread);

  // we have current thread context, so we can setup proper ISRs.
  pic_stage_3_init();
  isr_stage_3_trap();
  isr_stage_3_syscall();

  // now that we have malloc, move early_data from stack to heap.
  kernel.early_data = malloc(sizeof(early_data_t));
  memcpy(kernel.early_data, data, sizeof(early_data_t));

  // from this point onwards, the original kernel stack will be reset
  // on context switches. initiazation will start using the stack of
  // the processing domain.
  scheduler_add_thread(kernel.processes[0]->thread);
  scheduler_goto_next();
}
