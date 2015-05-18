#ifndef _CORESRV_DEV_H_
#define _CORESRV_DEV_H_

#include <coresrv/coresrv.h>

void dev_init();

__IDC__ int dev_register(IDC_PTR, const char*, struct vfs_filesys *);
__IDC__ int dev_unregister(IDC_PTR, const char*);


#endif
