#include <dirent.h>
#include <unistd.h>
#include <errno.h>

struct dirent *readdir(DIR *dirp) {
  if (!dirp) {
    errno = EINVAL;
    return NULL;
  }
  int i = read(dirp->fd, &dirp->de, sizeof(struct dirent));
  if (i == sizeof(struct dirent)) {
    return &dirp->de;
  } else {
    return NULL;
  }
}

int readdir_r(DIR * dirp, struct dirent * entry, struct dirent ** result) {
  if (!dirp) {
    errno = EINVAL;
    return -1;
  }
  int i = read(dirp->fd, entry, sizeof(struct dirent));
  if (i == sizeof(struct dirent)) {
    *result = entry;
    return 0;
  } else {
    *result = NULL;
    return 0;
  }
}
