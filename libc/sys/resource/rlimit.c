#include <sys/resource.h>
#include <limits.h>
#include <errno.h>

int getrlimit(int resource, struct rlimit *rlp) {
  switch (resource) {
  case RLIMIT_CORE:
    rlp->rlim_cur = rlp->rlim_max = RLIM_INFINITY;
    return 0;
  case RLIMIT_CPU:
    rlp->rlim_cur = rlp->rlim_max = RLIM_INFINITY;
    return 0;
  case RLIMIT_DATA:
    rlp->rlim_cur = rlp->rlim_max = RLIM_INFINITY;
    return 0;
  case RLIMIT_FSIZE:
    rlp->rlim_cur = rlp->rlim_max = RLIM_INFINITY;
    return 0;
  case RLIMIT_NOFILE:
    rlp->rlim_cur = rlp->rlim_max = OPEN_MAX;
    return 0;
  case RLIMIT_STACK:
    rlp->rlim_cur = rlp->rlim_max = 0x100000;
    return 0;
  case RLIMIT_AS:
    rlp->rlim_cur = rlp->rlim_max = RLIM_INFINITY;
    return 0;
  default:
    errno = EINVAL;
    return -1;
  }
}

int setrlimit(int resource, const struct rlimit *rlp) {
  (void)(resource);
  (void)(rlp);
  // TODO: implement actual limits.
  return 0;
}
