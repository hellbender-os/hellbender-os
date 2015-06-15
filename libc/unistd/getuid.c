#include <unistd.h>

#include <kernel/thread.h>

uid_t getuid(void) {
  return THREAD_INFO->uid;
}

uid_t geteuid(void) {
  return THREAD_INFO->euid;
}

