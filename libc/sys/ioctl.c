#include <sys/ioctl.h>

int ioctl(int fildes, int request, ...) {
  (void)(fildes);
  (void)(request);
  return -1;
}
