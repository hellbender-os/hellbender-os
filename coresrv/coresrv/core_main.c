#include <stdio.h>
#include <sched.h>
#include <semaphore.h>

extern void ps2_init();
extern void kbd_init();
extern void rtc_init();
extern void vga_init();

int main(void) {
  printf("Core pre-init begins!\n");
  sem_t* to_core = sem_open("kernel_to_core", 0);
  sem_t* to_kernel = sem_open("core_to_kernel", 0);

  // do initialization without interrupts.

  ps2_init();
  kbd_init();
  rtc_init();
  vga_init();
  
  printf("Core pre-init done.\n");
  sem_post(to_kernel);
  
  // kernel will enable interrupts.
  
  sem_wait(to_core);
  printf("Core post-init begins.\n");

  // do initialization with interrupts.

  printf("Core post-init done.\n");
  sem_post(to_kernel);

  while (1) sched_yield();
}
