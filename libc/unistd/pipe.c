#include <unistd.h>
#include <errno.h>

#include "../fcntl/fcntl_impl.h"

int pipe(int fildes[2])  {
  int handle1 = -1;
  int handle2 = -1;
  
  handle1 = fcntl_allocate_handle();
  if (handle1 < 0) {
    errno = EMFILE;
    goto error;
  }
  handle2 = fcntl_allocate_handle();
  if (handle2 < 0) {
    errno = EMFILE;
    goto error;
  }

  struct vfs_file *file1 = &_fcntl_data.handles[handle1];
  struct vfs_file *file2 = &_fcntl_data.handles[handle2];

  if (CORE_IDC(vfs_pipe, file1, file2) != 0) {
    goto error;
  }

  fildes[0] = handle1;
  fildes[1] = handle2;
  return 0;

 error:
  if (handle1 >= 0) fcntl_release_handle(handle1);
  if (handle2 >= 0) fcntl_release_handle(handle2);
  return -1;
}
