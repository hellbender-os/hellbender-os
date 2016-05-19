#include <termios.h>
#include <errno.h>

#include <hellbender/fs/vfs.h>
#include <hellbender/dev/tty.h>
#include <hellbender/fcntl_impl.h>

int tcsetattr(int fildes, int action, const struct termios *t) {
  vfs_file_t *file = _fcntl_get_file(fildes);
  if (!file) {
    errno = EBADF;
    return -1;
  }
  tty_t tty;
  if (file->op.query(file, VFS_IFACE_TTY, &tty)) {
    errno = ENOTTY;
    return -1;
  }
  errno = tty.op.setattr(&tty, action, t);
  return errno ? -1 : 0;
}
