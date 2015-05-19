#include <limits.h>
#include <coresrv/vfs.h>

struct fcntl_data {
  struct vfs_file handles[OPEN_MAX];
};

extern struct fcntl_data _fcntl_data;

static inline int fcntl_find_handle() {
  for (int i = 0; i < OPEN_MAX; ++i) {
    if (!_fcntl_data.handles[i].filesys.open) return i;
  }
  return -1;
}
