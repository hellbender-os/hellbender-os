#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>

#include "../fcntl/fcntl_impl.h"

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
           const char *path2, int flag) {
  if (strlen(path1) == 0 || strlen(path2) == 0) {
    errno = ENOENT;
    return -1;
  }
  int oflag = O_NOFOLLOW;
  if (flag == AT_SYMLINK_FOLLOW) oflag = 0;
  else if (flag) {
    errno = EINVAL;
    return -1;
  }
  struct vfs_file *dir1 = NULL;
  if (fd1 >= 0 && fd1 <= OPEN_MAX) {
    dir1 = &_fcntl_data.handles[fd1];
  }
  struct vfs_file *dir2 = NULL;
  if (fd2 >= 0 || fd2 <= OPEN_MAX) {
    dir2 = &_fcntl_data.handles[fd2];
  }
  struct vfs_file file1;
  if (CORE_IDC(vfs_resolve, dir1, &file1, path1, O_RDONLY | oflag) != 0) {
    return -1;
  }
  if (!file1.stat.st_mode) {
    errno = ENOENT;
    return -1;
  }
  struct vfs_file file2;
  if (CORE_IDC(vfs_resolve, dir2, &file2, path2, O_WRONLY | O_NOFOLLOW) != 0) {
    return -1;
  }
  if (file1.stat.st_dev != file2.stat.st_dev) {
    errno = EXDEV;
    return -1;
  }
  if (file2.stat.st_mode) {
    errno = EEXIST;
    return -1;
  }
  if (!file2.filesys.link) {
    errno = EROFS;
    return -1;
  }
  if (IDC(vfs_link, file2.filesys.link, &file2, &file1) != 0) {
    return -1;
  }
  return 0;
}
