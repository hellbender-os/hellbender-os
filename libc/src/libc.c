#include <errno.h>
#include <hellbender/ctrl.h>
#include <hellbender/fs/vfs.h>

__thread int errno;
__thread int __hellbender_jmpval;

struct ctrl ctrl;
vfs_t vfs;
