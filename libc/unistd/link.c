#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>

#include <hellbender/fs/vfs.h>
#include <hellbender/fcntl_impl.h>

int link(const char *path1, const char *path2) {
  int fd1 = -1;
  int fd2 = -1;
  if (path1[0] != VFS_SEPARATOR_CHAR) {
    fd1 = AT_FDCWD;
  }
  if (path2[0] != VFS_SEPARATOR_CHAR) {
    fd2 = AT_FDCWD;
  }
  return linkat(fd1, path1, AT_FDCWD, path2, fd2);
}

int linkat(int fd1, const char *path1, int fd2,
           const char *path2, int atflags) {
  if (strlen(path1) == 0 || strlen(path2) == 0) {
    errno = ENOENT;
    return -1;
  }
  if (atflags & ~AT_SYMLINK_FOLLOW) {
    errno = EINVAL;
    return -1;
  }
  vfs_file_t *dir1 = _fcntl_get_file(fd1);
  if (path1[0] == VFS_SEPARATOR_CHAR) {
    dir1 = 0;
  } else if (!dir1 || !(dir1->oflags & (O_RDONLY | O_SEARCH))) {
    errno = EBADF;
    return -1;
  }
  vfs_file_t *dir2 = _fcntl_get_file(fd2);
  if (path2[0] == VFS_SEPARATOR_CHAR) {
    dir2 = 0;
  } else if (!dir2 || !(dir2->oflags & (O_RDONLY | O_SEARCH))) {
    errno = EBADF;
    return -1;
  }
  vfs_file_t file1;
  if (!(atflags & AT_SYMLINK_FOLLOW)) atflags = AT_SYMLINK_NOFOLLOW;
  errno = vfs.open(dir1, path1, 0, atflags, 0, &file1);
  if (errno) return -1;
  errno = vfs.link(dir2, path2, &file1);
  file1.op.close(&file1);
  return errno ? -1 : 0;
}
