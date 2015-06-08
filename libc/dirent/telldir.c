#include <dirent.h>
#include <unistd.h>
#include <errno.h>

long telldir(DIR *dirp) {
  if (!dirp) {
    errno = EINVAL;
    return 0;
  }
  return (long)lseek(dirp->fd, 0, SEEK_CUR);
}
