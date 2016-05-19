#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>

#include <hellbender/fcntl_impl.h>
#include <hellbender/fs/vfs.h>

int symlink(const char *path1, const char *path2) {
  int fd = -1;
  if (path2[0] != VFS_SEPARATOR_CHAR) {
    fd = AT_FDCWD;
  }
  return symlinkat(path1, fd, path2);
}

int symlinkat(const char *path1, int fd, const char *path2) {
  if (strlen(path2) == 0) {
    errno = ENOENT;
    return -1;
  }
  vfs_file_t *dir = _fcntl_get_file(fd);
  if (path2[0] == VFS_SEPARATOR_CHAR) {
    dir = 0;
  } else if (!dir || !(dir->oflags & (O_RDONLY | O_SEARCH))) {
    errno = EBADF;
    return -1;
  }
  vfs_file_t file;
  errno = vfs.open(dir, path2, O_CREAT | O_EXCL | O_WRONLY, 0, 
                   S_IFLNK | S_IRUSR|S_IRGRP|S_IROTH, &file);
  if (errno) return -1;
  size_t size = strlen(path1);
  errno = file.op.write(&file, 0, path1, &size);
  file.op.close(&file);
  return errno ? -1 : 0;
}
