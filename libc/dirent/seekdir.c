#include <dirent.h>
#include <unistd.h>
#include <errno.h>

void seekdir(DIR *dirp, long loc) {
  if (dirp) {
    errno = EINVAL;
  }
  lseek(dirp->fd, loc, SEEK_SET);
}
