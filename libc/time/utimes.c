#include <sys/time.h>
#include <fcntl.h>
#include <coresrv/vfs.h>

int utimes(const char *path, const struct timeval times[2]) {
  int fd = -1;
  if (path[0] != VFS_SEPARATOR_CHAR) {
    fd = AT_FDCWD;
  }
  struct timespec specs[2] = {
    { times[0].tv_sec, 1000l * (long)times[0].tv_usec },
    { times[1].tv_sec, 1000l * (long)times[1].tv_usec },
  };
  return utimensat(fd, path, specs, 0);
}
