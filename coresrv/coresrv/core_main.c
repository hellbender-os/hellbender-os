#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <fcntl.h>
#include <unistd.h>
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
volatile int init_stage = 0;

void core_initialize() {
  //printf("Core pre-init begins!\n");

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
  init_stage = CORE_INIT_STAGE_READY_FOR_INTERRUPTS;
  
  // kernel will enable interrupts.
  while (init_stage != CORE_INIT_STAGE_INTERRUPTS_ACTIVATED) sched_yield();

  // continue with interrupts activated.
  // we'll return into _start, which will finish libC init
  // and then calls core_main.
}

int core_main() {
  printf("Core init done; launching init.\n");
  pid_t init_pid;
  char* argv[] = { "init", "tty=devfs/tty2", (char*)0 };
  char* envp[] = { "PATH=test", "kissa=koira", (char*)0 };
  posix_spawn(&init_pid, "initfs/init", NULL, NULL, argv, envp);

  exit(1);
}
