#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

struct _linkedlist _stdio_files;

FILE _stdin  = { .buffer=NULL, .buf_size=BUFSIZ, .buf_bytes=0,
                 .buf_offset=0, .fd=-1, .row=1, .error=0 };
FILE _stdout = { .buffer=NULL, .buf_size=BUFSIZ, .buf_bytes=0,
                 .buf_offset=0, .fd=-1, .row=0, .error=0 };
FILE _stderr = { .buffer=NULL, .buf_size=BUFSIZ, .buf_bytes=0,
                 .buf_offset=0, .fd=-1, .row=0, .error=0 };

FILE *stdin = NULL;
FILE *stdout = NULL;
FILE *stderr = NULL;

void stdio_init() {
  // init FILE* API.
  _ll_init(&_stdio_files);
  _stdin.buffer = (char*)malloc(BUFSIZ);
  _stdout.buffer = (char*)malloc(BUFSIZ);
  _stderr.buffer = (char*)malloc(BUFSIZ);
  // TODO: get the actual tty name from somewhere.
  _stdin.fd = openat(-1, "devfs/tty1", 0);
  _stdout.fd = openat(-1, "devfs/tty1", 0);
  _stderr.fd = openat(-1, "devfs/tty1", 0);

  if (STDIN_FILENO != _stdin.fd
      || STDOUT_FILENO != _stdout.fd
      || STDERR_FILENO != _stderr.fd) {
    printf("stdio_init failed!\n");
    abort();
  }
  
  stdin = &_stdin;
  stdout = &_stdout;
  stderr = &_stderr;
  _LL_INSERT(&_stdio_files, stdin);
  _LL_INSERT(&_stdio_files, stdout);
  _LL_INSERT(&_stdio_files, stderr);
}
