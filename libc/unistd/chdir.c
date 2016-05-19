#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#include <hellbender/fcntl_impl.h>
#include <hellbender/fs/vfs.h>

int chdir(const char *path) {
  if (strlen(path) == 0) {
    errno = ENOENT;
    return -1;
  }
  vfs_file_t *cur_dir = _fcntl_get_file(AT_FDCWD);
  if (path[0] == VFS_SEPARATOR_CHAR) {
    cur_dir = 0;
  }
  vfs_file_t new_dir;
  errno = vfs.open(cur_dir, path, O_DIRECTORY | O_SEARCH, 0, 0, &new_dir);
  if (errno) return -1;
  errno = cur_dir->op.close(cur_dir);
  if (!errno) *cur_dir = new_dir;
  return errno ? -1 : 0;
}

int fchdir(int fildes) {
  vfs_file_t *new_dir = _fcntl_get_file(fildes);
  if (!new_dir || !(new_dir->oflags)) {
    errno = EBADF;
    return -1;
  }
  if (!S_ISDIR(new_dir->st_mode)) {
    errno = ENOTDIR;
    return -1;
  }
  errno = vfs.access(new_dir, O_SEARCH);
  if (!errno) errno = dup2(fildes, AT_FDCWD);
  return errno ? -1 : 0;
}
