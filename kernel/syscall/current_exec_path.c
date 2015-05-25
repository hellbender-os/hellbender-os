#include <string.h>
#include <kernel/syscall.h>
#include <kernel/thread.h>

void syscall_current_exec_path(char* path) {
  if (CURRENT_THREAD->exec_path) {
    strcpy(path, CURRENT_THREAD->exec_path);
  } else {
    *path = 0;
  }
}
