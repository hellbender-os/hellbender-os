#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

int mknod(const char *path, mode_t mode, dev_t dev) {
  (void)(dev);
  if (S_ISDIR(mode)) {
    return mkdir(path, mode);
  } else if (S_ISFIFO(mode)) {
    return mkfifo(path, mode);
  } else if (S_ISREG(mode)) {
    return open(path, O_WRONLY|O_CREAT|O_TRUNC, mode);
  } else {
    errno = EINVAL;
    return -1;
  }
}

int mknodat(int fd, const char *path, mode_t mode, dev_t dev) {
  (void)(dev);
  if (S_ISDIR(mode)) {
    return mkdirat(fd, path, mode);
  } else if (S_ISFIFO(mode)) {
    return mkfifoat(fd, path, mode);
  } else if (S_ISREG(mode)) {
    return openat(fd, path, O_WRONLY|O_EXCL|O_CREAT, mode);
  } else {
    errno = EINVAL;
    return -1;
  }
}
