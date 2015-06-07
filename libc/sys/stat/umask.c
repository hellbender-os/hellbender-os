#include <sys/stat.h>

#include <../fcntl/fcntl_impl.h>

mode_t umask(mode_t cmask) {
  mode_t retval = _fcntl_data.umask;
  _fcntl_data.umask = cmask | ~0777;
  return retval;
}
