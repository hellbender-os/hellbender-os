#include <dirent.h>
#include <errno.h>

int dirfd(DIR *dirp) {
  if (!dirp) {
    errno = EINVAL;
    return -1;
  }
  return dirp->fd;
}
