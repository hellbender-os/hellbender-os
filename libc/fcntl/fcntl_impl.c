#include <hellbender/fcntl_impl.h>
#include <hellbender/fence.h>
#include <stdlib.h>
#include <string.h>

struct _fcntl _fcntl;

int _fcntl_reserve_fildes(int min_fildes) {
  for (int fildes = min_fildes; fildes < OPEN_MAX; ++fildes) {
    _fd_t* fd = _fcntl_get_fd(fildes);
    if (__sync_bool_compare_and_swap(&fd->status, 0, 1)) {
      return fildes;
    }
  }
  return -1;
}

void _fcntl_release_fildes(int fildes) {
  _fd_t* fd = _fcntl_get_fd(fildes);
  if (__sync_bool_compare_and_swap(&fd->status, 1, -1)) {
    fd->file = (vfs_file_t){0};
    fd->offset = 0;
    fd->flags = 0;
    FENCE; // setting to zero allows others to reserve it, so cleanup must happen first.
    fd->status = 0;
  }
}

_fd_t* _fcntl_peek_fd2(int fildes) {
  if (fildes == OPEN_MAX) return &_fcntl.cwd;
  else if (fildes < 0 || fildes > OPEN_MAX) return 0;
  else if (fildes < HANDLE_GROUP) return _fcntl.fildes + fildes;
  else {
    int group = fildes / HANDLE_GROUP - 1;
    int g_offset = fildes % HANDLE_GROUP;
    _fd_t* group_ptr = _fcntl.indirect_fildes[group];
    if (!group_ptr) return 0;
    else return group_ptr[g_offset].status == 1 ? group_ptr + g_offset : 0;
  }
}

_fd_t* _fcntl_get_fd2(int fildes) {
  if (fildes == OPEN_MAX) return &_fcntl.cwd;
  else if (fildes < 0 || fildes > OPEN_MAX) return 0;
  else if (fildes < HANDLE_GROUP) return _fcntl.fildes + fildes;
  else {
    int group = fildes / HANDLE_GROUP - 1;
    int g_offset = fildes % HANDLE_GROUP;
    _fd_t* group_ptr = _fcntl.indirect_fildes[group];
    if (!group_ptr) {
      group_ptr = calloc(HANDLE_GROUP, sizeof(*group_ptr));
      _fd_t* test_ptr = 
        __sync_val_compare_and_swap(_fcntl.indirect_fildes + group, 0, group_ptr);
      if (!test_ptr) { // someone was able to create this before us, roll back..
        free(group_ptr);
        group_ptr = test_ptr;
      }
    }
    return group_ptr + g_offset;
  }
}
