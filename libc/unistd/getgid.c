#include <unistd.h>

#include <kernel/thread.h>

gid_t getgid(void) {
  return THREAD_INFO->gid;
}

gid_t getegid(void) {
  return THREAD_INFO->egid;
}

