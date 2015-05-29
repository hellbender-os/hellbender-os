#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

FILE _stdin  =
  { .buffer=NULL, .buf_size=BUFSIZ, .buf_bytes=0, .fd=-1, .row=1 };
FILE _stdout =
  { .buffer=NULL, .buf_size=BUFSIZ, .buf_bytes=0, .fd=-1, .row=0 };
FILE _stderr =
  { .buffer=NULL, .buf_size=BUFSIZ, .buf_bytes=0, .fd=-1, .row=0 };

FILE *stdin = NULL;
FILE *stdout = NULL;
FILE *stderr = NULL;

void stdio_init() {
  // init FILE* API.
  _stdin.buffer = (char*)malloc(BUFSIZ);
  _stdout.buffer = (char*)malloc(BUFSIZ);
  _stderr.buffer = (char*)malloc(BUFSIZ);
  // TODO: get the actual tty name from somewhere.
  _stdin.fd = open("devfs/tty1", 0);
  _stdout.fd = open("devfs/tty1", 0);
  _stderr.fd = open("devfs/tty1", 0);

  if (STDIN_FILENO != _stdin.fd
      || STDOUT_FILENO != _stdout.fd
      || STDERR_FILENO != _stderr.fd) {
    printf("stdio_init failed!\n");
    abort();
  }
  
  stdin = &_stdin;
  stdout = &_stdout;
  stderr = &_stderr;
}
