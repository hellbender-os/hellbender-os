#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

FILE _stdin = { NULL, 0, STDIN_FILENO };
FILE _stdout = { NULL, 0, STDOUT_FILENO };
FILE _stderr = { NULL, 0, STDERR_FILENO };

void stdio_init() {
  // init FILE* API.
  _stdin.buffer = (char*)malloc(BUFSIZ);
  _stdout.buffer = (char*)malloc(BUFSIZ);
  _stderr.buffer = (char*)malloc(BUFSIZ);
  // TODO: get the actual tty name from somewhere.
  int fd0 = open("devfs/tty1", 0);
  int fd1 = open("devfs/tty1", 0);
  int fd2 = open("devfs/tty1", 0);

  if (fd0 != _stdin.fd || fd1 != _stdout.fd || fd2 != _stderr.fd) {
    printf("stdio_init failed!\n");
    abort();
  }
}
