#include <limits.h>
#include <coresrv/vfs.h>

struct fcntl {
  vfo_t handles[OPEN_MAX];
};

extern struct fcntl fcntl_data;

static inline int fcntl_find_handle() {
  for (int i = 0; i < OPEN_MAX; ++i) {
    if (!fcntl_data.handles[i].id) return i;
  }
  return -1;
}
