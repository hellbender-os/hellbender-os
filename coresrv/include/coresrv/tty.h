#ifndef _CORESRV_TTY_H
#define _CORESRV_TTY_H

#include <coresrv/vfs.h>

void dev_tty_init();
void dev_tty_switch_to(unsigned tty_id);

//TODO: add native API for TTYs.

int dev_tty_open(IDC_PTR, struct vfs_file*, const char*, int);
int dev_tty_close(IDC_PTR, struct vfs_file*);
ssize_t dev_tty_read(IDC_PTR, struct vfs_file*, void*, size_t);
ssize_t dev_tty_write(IDC_PTR, struct vfs_file*, const void*, size_t);
off_t dev_tty_lseek(IDC_PTR, struct vfs_file*, off_t, int);

#endif
