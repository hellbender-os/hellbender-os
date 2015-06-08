#include <dirent.h>
#include <unistd.h>
#include <errno.h>

void rewinddir(DIR *dirp) {
  if (!dirp) {
    errno = EINVAL;
  }
  lseek(dirp->fd, 0, SEEK_SET);
}
