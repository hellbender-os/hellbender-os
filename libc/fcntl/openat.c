#include <stdio.h>
#include <fcntl.h>
#include <stdarg.h>
#include <errno.h>
#include <string.h>
#include <limits.h>

#include "fcntl_impl.h"

int openat(int dir_fd, const char *name, int flags, ...) {
  // check dir_fd
  struct vfs_file *dir = NULL;
  /* POSIX says that name is relative when it doesn't start with a slash.
   * If so, then dir_fd MUST be valid.
   * However, Hellbender filesystem is a forest, so we do it other way around:
   * If dir_fd is not valid, THEN name is considered absolute.
   */
  /* This would be POSIX conforming, but prevents the filesystem forest:
  if (name[0] != VFS_SEPARATOR_CHAR) {
    if (dir_fd < 0 || dir_fd > OPEN_MAX) {
      errno = EBADF;
      return -1;
    }
    dir = &_fcntl_data.handles[dir_fd];
  }
  */
  if (dir_fd >= 0 && dir_fd <= OPEN_MAX) {
    dir = &_fcntl_data.handles[dir_fd];
  }

  // check file length
  /* POSIX says that name cannot be empty.
   * However, Hellbender filesystem root is "".
   * So, we forbid only empty relative names.
   */
  if (strlen(name) == 0 && dir) {
    errno = ENOENT;
    return -1;
  }
  
  // allocate virtual file object.
  int handle = fcntl_allocate_handle();
  if (handle < 0) {
    errno = EMFILE;
    goto openat_error;
  }

  // resolve the path:
  struct vfs_file *file = &_fcntl_data.handles[handle];
  if (CORE_IDC(vfs_resolve, dir, file, name, flags) != 0) {
    goto openat_error;
  }

  // existing file should have been opened by the resolve.
  if (!(flags & O_CREAT) && !file->stat.st_mode) {
    errno = ENOENT;
    goto openat_error;
  }

  // O_EXEC, O_RDONLY, O_RDWD, O_SEARCH, O_WRONLY handled by vfs_resolve.
  // O_APPEND handled by write.
  // O_CLOEXEC handled by exec*.
  // O_NOFOLLOW handled by vfs_resolve.
  // O_NONBLOCK handled by vfs_open.
  // O_RSYNC handled by vfs_read.
  // O_SYNC handled by vfs_write.

  if (flags & O_DIRECTORY && !S_ISDIR(file->stat.st_mode)) {
    errno = ENOTDIR;
    goto openat_error;
  }
  if (flags & O_WRONLY) {
    if (S_ISDIR(file->stat.st_mode)) {
      errno = EISDIR;
      goto openat_error;
    }
  }
  
  // O_CREAT & O_EXCL:
  if (flags & O_CREAT) {
    if (file->stat.st_mode) {
      if (flags & O_EXCL) { // file exists & O_EXCL set.
        errno = EEXIST;
        goto openat_error;
      }
    } else { // file does not exist.
      va_list parameters;
      va_start(parameters, flags);
      mode_t mode = va_arg(parameters, mode_t);
      va_end(parameters);
      if (!file->filesys.create) {
        errno = EROFS;
        goto openat_error;
      }
      if (IDC(vfs_create, file->filesys.create, file, mode) != 0) {
        goto openat_error;
      }
    }
  }
  
  // File is now open, or we have failed.
  if (flags & O_TTY_INIT && file->filesys.termios) {
    // TODO: initializer struct termios.
    // See: http://pubs.opengroup.org/onlinepubs/9699919799/basedefs/V1_chap11.html#tag_11_02
  }
  if (!(flags & O_NOCTTY) && file->filesys.termios) {
    // TODO: set as controlling TTY if this is a session leader and tty is free.
    // See: http://pubs.opengroup.org/onlinepubs/000095399/basedefs/xbd_chap11.html#tag_11_01_03
  }
  if (flags & O_WRONLY && flags & O_TRUNC) {
    if (!file->filesys.ftruncate) {
      errno = EROFS;
      goto openat_error;
    }
    if (IDC(vfs_ftruncate, file->filesys.ftruncate, file, 0) != 0) {
      goto openat_error;
    }
  }
  
  return handle;

 openat_error:
  fcntl_release_handle(handle);
  return -1;
}
