#include <unistd.h>

#include <hellbender/ctrl.h>

gid_t getgid(void) {
  return ctrl.get_gid();
}

gid_t getegid(void) {
  return ctrl.get_egid();
}

