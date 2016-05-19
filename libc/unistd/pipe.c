#include <unistd.h>
#include <errno.h>

#include <hellbender/fcntl_impl.h>
#include <hellbender/fs/vfs.h>

int pipe(int fildes[2])  {
  int fildes1 = -1;
  int fildes2 = -1;
  
  fildes1 = _fcntl_reserve_fildes(0);
  if (fildes1 < 0) {
    errno = EMFILE;
    goto error;
  }
  fildes2 = _fcntl_reserve_fildes(0);
  if (fildes2 < 0) {
    errno = EMFILE;
    goto error;
  }

  _fd_t *fd1 = _fcntl_get_fd(fildes1);
  _fd_t *fd2 = _fcntl_get_fd(fildes2);
  errno = vfs.pipe(&fd1->file, &fd2->file);
  if (errno) goto error;
  fildes[0] = fildes1;
  fildes[1] = fildes2;
  return 0;

 error:
  if (fildes1 >= 0) _fcntl_release_fildes(fildes1);
  if (fildes2 >= 0) _fcntl_release_fildes(fildes2);
  return -1;
}
