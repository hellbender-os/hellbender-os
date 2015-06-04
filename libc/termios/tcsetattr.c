#include <termios.h>
#include <errno.h>

#include <coresrv/vfs.h>

#include "../fcntl/fcntl_impl.h"

int tcsetattr(int fildes, int action, const struct termios *termios_p) {
  if (action != 0 && action != TCSANOW
      && action != TCSADRAIN && action != TCSAFLUSH) {
    errno = EINVAL;
    return -1;
  }
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
  if (IDC(vfs_termios, file->filesys.termios, file,
          (struct termios *)termios_p, VFS_TERMIOS_SET | action) != 0) {
    return -1;
  } else {
    return 0;
  }
}
