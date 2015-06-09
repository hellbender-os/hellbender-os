#include <stdio.h>
#include <spawn.h>

int main(void) {
  printf("Init launching shells on terminals TTY2, TTY3\n");
  {
    pid_t pid;
    char* argv[] = { "hellish", "devfs/tty1", (char*)0 };
    char* envp[] = { "hell=1", (char*)0 };
    posix_spawn(&pid, "initrd/bin/hellish", NULL, NULL, argv, envp);
  }
  {
    pid_t pid;
    char* argv[] = { "hellish", "devfs/tty3", (char*)0 };
    char* envp[] = { "hell=2", (char*)0 };
    posix_spawn(&pid, "initrd/bin/hellish", NULL, NULL, argv, envp);
  }
  
  return 2;
}
