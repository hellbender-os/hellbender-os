#include <termios.h>
#include <errno.h>

#include <coresrv/vfs.h>

#include "../fcntl/fcntl_impl.h"

int tcgetattr(int fildes, struct termios *termios_p) {
  if (fildes < 0 || fildes >= OPEN_MAX) {
    errno = EBADF;
    return -1;
  }
  struct vfs_file *file = &_fcntl_data.handles[fildes];
  if (!file->stat.st_mode) {
    errno = EBADF;
    return -1;
  }
  if (!file->filesys.termios) {
    errno = ENOTTY;
    return -1;
  }
  if (IDC(vfs_termios, file->filesys.termios, file, termios_p,
          VFS_TERMIOS_GET) != 0) {
    return -1;
  } else {
    return 0;
  }
}
