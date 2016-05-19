#include <sys/stat.h>

#include <hellbender/fcntl_impl.h>

mode_t umask(mode_t cmask) {
  mode_t retval = _fcntl.umask;
  _fcntl.umask = cmask | ~0777;
  return retval;
}
