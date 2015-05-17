#include <limits.h>
#include <coresrv/vfs.h>

struct fcntl_data {
  struct vfs_file handles[OPEN_MAX];
};

extern struct fcntl_data fcntl_data;

static inline int fcntl_find_handle() {
  for (int i = 0; i < OPEN_MAX; ++i) {
    if (!fcntl_data.handles[i].filesys) return i;
  }
  return -1;
}
