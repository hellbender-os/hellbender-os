#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>
#include <spawn.h>

#include <kernel/kernel.h>
#include <kernel/module.h>
#include <coresrv/vfs.h>
#include <coresrv/devfs.h>
#include <coresrv/initfs.h>
#include <coresrv/tty.h>

extern void ps2_init();
extern void kbd_init();
extern void rtc_init();
extern void vga_init();
extern void vfs_init();
extern void dev_init();

struct vfs_initfs vfs_initfs;

void core_initialize() {
  //printf("Core pre-init begins!\n");
  sem_t* to_core = sem_open("kernel_to_core", 0);
  sem_t* to_kernel = sem_open("core_to_kernel", 0);

  // do initialization without interrupts.

  kbd_init();
  ps2_init();
  rtc_init();
  vga_init();
  vfs_init();
  dev_init();

  printf("Setting up virtual consoles.\n");
  dev_tty_init();

  printf("Mounting initrd.\n");
  vfs_initfs_init(&vfs_initfs,
                  (uint8_t*)CORE_SERVICE->initrd_buffer,
                  (size_t)CORE_SERVICE->initrd_size);
  if (vfs_mount(NO_IDC, "initfs/", &vfs_initfs.filesys)) {
    printf("Cannot mount init file system.\n");
    abort();
  }

  // let kernel know that pre-init is done.
  sem_post(to_kernel);
  
  // kernel will enable interrupts.
  sem_wait(to_core);

  // do initialization with interrupts.

  // let kernel know that post-init is done.
  sem_post(to_kernel);

  // let kernel finish TODO: could we wait for the kernel thread to exit?
  for (int i = 0; i < 100; ++i) sched_yield();
}

int core_main() {
  printf("Core init done; launching init.\n");
  pid_t init_pid;
  char* argv[] = { "init", (char*)0 };
  char* envp[] = { (char*)0 };
  posix_spawn(&init_pid, "initfs/init", NULL, NULL, argv, envp);

  exit(1);
}
