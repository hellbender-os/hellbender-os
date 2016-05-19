#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#include <hellbender/fcntl_impl.h>

int futimens(int fd, const struct timespec times[2]) {
  vfs_file_t *file = _fcntl_get_file(fd);
  int oflags = 0;
  if (times[0].tv_nsec != UTIME_OMIT) oflags |= O_RDONLY;
  if (times[1].tv_nsec != UTIME_OMIT) oflags |= O_WRONLY;
  if (!file || ((file->oflags & oflags) != oflags)) {
    errno = EBADF;
    return -1;
  }
  errno = file->op.utimens(file, times);
  return errno ? -1 : 0;
}

int utimensat(int fd, const char *path, const struct timespec times[2], int flag) {
  if (strlen(path) == 0) {
    errno = ENOENT;
    return -1;
  }
  if (flag & ~AT_SYMLINK_NOFOLLOW) {
    errno = EINVAL;
    return -1;
  }
  vfs_file_t *dir = _fcntl_get_file(fd);
  if (path[0] == VFS_SEPARATOR_CHAR) {
    dir = 0;
  } else if (!dir || !(dir->oflags & (O_RDONLY | O_SEARCH))) {
    errno = EBADF;
    return -1;
  }
  int oflags = 0;
  if (times[0].tv_nsec != UTIME_OMIT) oflags |= O_RDONLY;
  if (times[1].tv_nsec != UTIME_OMIT) oflags |= O_WRONLY;
  vfs_file_t file;
  errno = vfs.open(dir, path, oflags, 0, 0, &file);
  if (errno) return -1;
  errno = file.op.utimens(&file, times);
  file.op.close(&file);
  return errno ? -1 : 0;
}

