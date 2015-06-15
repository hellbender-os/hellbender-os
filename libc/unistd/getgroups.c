#include <unistd.h>

// TODO: support more than one group..

int getgroups(int gidsetsize, gid_t grouplist[]) {
  if (gidsetsize) {
    grouplist[0] = getegid();
  }
  return 1;
}

