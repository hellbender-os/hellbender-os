#include <unistd.h>
#include <errno.h>

#include <hellbender/fs/vfs.h>
#include <hellbender/dev/tty.h>
#include <hellbender/fcntl_impl.h>

int isatty(int fildes) {
  vfs_file_t *file = _fcntl_get_file(fildes);
  if (!file) {
    errno = EBADF;
    return 0;
  }
  tty_t tty;
  if (file->op.query(file, VFS_IFACE_TTY, &tty)) {
    errno = ENOTTY;
    return 0;
  } else {
    return 1;
  }
}

