#include <unistd.h>

#include <hellbender/ctrl.h>

uid_t getuid(void) {
  return ctrl.get_uid();
}

uid_t geteuid(void) {
  return ctrl.get_euid();
}
