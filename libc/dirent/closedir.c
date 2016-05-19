#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>

int closedir(DIR *dirp) {
  if (!dirp) {
    errno = EINVAL;
    return -1;
  }
  int retval = close(dirp->fd);
  free(dirp);
  return retval;
}
