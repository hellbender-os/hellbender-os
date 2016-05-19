#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#include <hellbender/fs/vfs.h>
#include <hellbender/fcntl_impl.h>

int chown(const char *path, uid_t user, gid_t group) {
  int fd = -1;
  if (path[0] != VFS_SEPARATOR_CHAR) {
    fd = AT_FDCWD;
  }
  return fchownat(fd, path, user, group, 0);
}

int lchown(const char *path, uid_t user, gid_t group) {
  int fd = -1;
  if (path[0] != VFS_SEPARATOR_CHAR) {
    fd = AT_FDCWD;
  }
  return fchownat(fd, path, user, group, AT_SYMLINK_NOFOLLOW);
}

int fchown(int fd, uid_t user, gid_t group) {
  vfs_file_t *file = _fcntl_get_file(fd);
  if (!file || !(file->oflags)) {
    errno = EBADF;
    return -1;
  }
  errno = vfs.chown(file, user, group);
  return errno ? -1 : 0;
}

int fchownat(int dir_fd, const char *path, uid_t user, gid_t group, int atflags) {
  if (strlen(path) == 0) {
    errno = ENOENT;
    return -1;
  }
  if (atflags & ~AT_SYMLINK_NOFOLLOW) {
    errno = EINVAL;
    return -1;
  }
  vfs_file_t *dir = _fcntl_get_file(dir_fd);
  if (path[0] == VFS_SEPARATOR_CHAR) {
    dir = 0;
  } else if (!dir || !(dir->oflags & (O_RDONLY | O_SEARCH))) {
    errno = EBADF;
    return -1;
  }
  vfs_file_t file;
  errno = vfs.open(dir, path, 0, atflags, 0, &file);
  if (errno) return -1;
  errno = vfs.chown(&file, user, group);
  file.op.close(&file);
  return errno ? -1 : 0;
}
