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

  VGA_AT(0,6) = VGA_ENTRY('n', WHITE_ON_BLACK);
  initfs_init();
  initfs_t initfs;
  INITFS_BIND(&initfs);
  vfs_tag_t init_root;
  if (initfs.create(data->initrd_base, data->initrd_size, &init_root)) exit(-1);
  if (vfs.mount("/", &init_root)) exit(-1);

  tmpfs_init();
  tmpfs_t tmpfs;
  TMPFS_BIND(&tmpfs);
  vfs_tag_t tmp_root;
  if (tmpfs.create(0x200000, &tmp_root)) exit(-1);
  if (vfs.mount("/tmp", &tmp_root)) exit(-1);

  devfs_init();
  devfs_t devfs;
  DEVFS_BIND(&devfs);
  vfs_tag_t dev_root;
  if (devfs.create(&dev_root)) exit(-1);
  if (vfs.mount("/dev", &dev_root)) exit(-1);
  VGA_AT(0,6) = VGA_ENTRY('N', WHITE_ON_BLACK);

  VGA_AT(0,7) = VGA_ENTRY('d', WHITE_ON_BLACK);
  console_init();
  console_t console;
  CONSOLE_BIND(&console);
  vfs_tag_t console_dev;
  if (console.create(&console_dev)) exit(-1);
  if (devfs.add_dev(&dev_root, "console", &console_dev)) exit(-1);
  VGA_AT(0,7) = VGA_ENTRY('D', WHITE_ON_BLACK);

  VGA_AT(0,8) = VGA_ENTRY('e', WHITE_ON_BLACK);
  ramdisk_init();
  ramdisk_t ramdisk;
  RAMDISK_BIND(&ramdisk);
  vfs_tag_t ramdisk_dev;
  if (ramdisk.create(0x200000, &ramdisk_dev)) exit(-1);
  if (devfs.add_dev(&dev_root, "ramdisk$", &ramdisk_dev)) exit(-1);
  
  fat16_init();
  fat16_t fat16;
  FAT16_BIND(&fat16);
  vfs_tag_t fat_root;
  if (fat16.create(&ramdisk_dev, &fat_root)) exit(-1);
  if (vfs.mount("/home", &fat_root)) exit(-1);
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
