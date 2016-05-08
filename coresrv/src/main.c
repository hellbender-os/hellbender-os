#include <hellbender/debug.h>
#include <hellbender/syscall.h>
#include <hellbender/libc_init.h>
#include <hellbender/coresrv_init.h>
#include <hellbender/broker.h>
#include <hellbender/fs/vfs.h>
#include <hellbender/fs/initfs.h>
#include <hellbender/fs/tmpfs.h>
#include <hellbender/fs/devfs.h>
#include <hellbender/fs/fat16.h>
#include <hellbender/dev/console.h>
#include <hellbender/dev/ramdisk.h>
#include "vga.h"

#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

void broker_init();
void vfs_init();
void initfs_init();
void tmpfs_init();
void devfs_init();
void fat16_init();
void console_init();
void ramdisk_init();

int main(int argc, char* argv[]) {
  (void)argc;
  (void)argv;
  struct coresrv_init *data = (struct coresrv_init *)_libc_init_get()->data;
  VGA_MEMORY((uint16_t*)data->vga_base);

  VGA_AT(0,5) = VGA_ENTRY('e', WHITE_ON_BLACK);
  broker_init();

  vfs_init();
  vfs_t vfs;
  VFS_BIND(&vfs);
  VGA_AT(0,5) = VGA_ENTRY('E', WHITE_ON_BLACK);

  devfs_init();
  devfs_op_t devfs_op;
  DEVFS_BIND(&devfs_op);
  vfs_fs_t devfs;
  if (devfs_op.create(&devfs)) exit(-1);
  if (vfs.mount("/dev", &devfs)) exit(-1);

  VGA_AT(0,6) = VGA_ENTRY('n', WHITE_ON_BLACK);
  initfs_init();
  initfs_op_t initfs_op;
  INITFS_BIND(&initfs_op);
  vfs_fs_t initfs;
  dev_t init_dev = devfs_op.alloc_dev();
  if (initfs_op.create(data->initrd_base, data->initrd_size, init_dev, &initfs)) exit(-1);
  if (vfs.mount("/", &initfs)) exit(-1);

  tmpfs_init();
  tmpfs_op_t tmpfs_op;
  TMPFS_BIND(&tmpfs_op);
  vfs_fs_t tmpfs;
  dev_t tmp_dev = devfs_op.alloc_dev();
  if (tmpfs_op.create(0x200000, tmp_dev, &tmpfs)) exit(-1);
  if (vfs.mount("/tmp", &tmpfs)) exit(-1);

  VGA_AT(0,6) = VGA_ENTRY('N', WHITE_ON_BLACK);

  VGA_AT(0,7) = VGA_ENTRY('d', WHITE_ON_BLACK);
  console_init();
  console_op_t console_op;
  CONSOLE_BIND(&console_op);
  vfs_node_t console;
  if (console_op.create(&console)) exit(-1);
  if (devfs_op.add_dev(&devfs, "console", &console, &console.stat.st_rdev)) exit(-1);
  VGA_AT(0,7) = VGA_ENTRY('D', WHITE_ON_BLACK);

  VGA_AT(0,8) = VGA_ENTRY('e', WHITE_ON_BLACK);
  ramdisk_init();
  ramdisk_op_t ramdisk_op;
  RAMDISK_BIND(&ramdisk_op);
  vfs_node_t ramdisk;
  if (ramdisk_op.create(0x200000, &ramdisk)) exit(-1);
  if (devfs_op.add_dev(&devfs, "ramdisk$", &ramdisk, &ramdisk.stat.st_rdev)) exit(-1);
  
  fat16_init();
  fat16_op_t fat16_op;
  FAT16_BIND(&fat16_op);
  vfs_fs_t fatfs;
  if (fat16_op.create(&ramdisk, &fatfs)) exit(-1);
  if (vfs.mount("/home", &fatfs)) exit(-1);
  VGA_AT(0,8) = VGA_ENTRY('E', WHITE_ON_BLACK);

  /*
  pid_t init = fork();
  if (!init) {
    execl(argv[1], argv[1], (void*)0);
  } else while (wait(0) == -1);
  */

  // test interrupt processing with signals.
  VGA_AT(0,9) = VGA_ENTRY('r', WHITE_ON_BLACK);
  syscall_set_signal_mask(~0ull);
  syscall_set_irq_signal(33, SIGUSR1);
  for (int signum = 0; !signum; ) {
    signum = syscall_wait_signal(1<<SIGUSR1);
  }
  VGA_AT(0,9) = VGA_ENTRY('R', WHITE_ON_BLACK);
}
