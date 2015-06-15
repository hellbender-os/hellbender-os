#include <unistd.h>

#include <kernel/thread.h>

pid_t getpid(void) {
  return THREAD_INFO->process_id;
}

pid_t getppid(void) {
  return THREAD_INFO->parent_id;
}
