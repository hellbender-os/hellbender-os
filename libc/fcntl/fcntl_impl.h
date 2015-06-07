#include <limits.h>
#include <coresrv/vfs.h>

struct fcntl_data {
  struct vfs_file handles[OPEN_MAX + 1]; // last one for AT_FDCWD
  mode_t umask;
};

extern struct fcntl_data _fcntl_data;

static inline int fcntl_allocate_handle_ge(int limit) {
  for (int i = limit; i < OPEN_MAX; ++i) {
    if (!_fcntl_data.handles[i].in_use) {
      unsigned test =
        __sync_lock_test_and_set(&_fcntl_data.handles[i].in_use, 1);
      if (!test) {
        return i;
      }
    }
  }
  return -1;
}

static inline int fcntl_allocate_handle() {
  return fcntl_allocate_handle_ge(0);
}

static inline void fcntl_release_handle(int handle) {
  if (handle >= 0 && handle < OPEN_MAX) {
    _fcntl_data.handles[handle].in_use = 0;
  }
}

