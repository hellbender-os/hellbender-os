#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

FILE *fopen(const char *name, const char *mode) {
  (void)(mode); //TODO: mode; support create.
  int flags = 0;
  if (strchr(mode, 'r')) flags = O_RDONLY;
  if (strchr(mode, 'w') || strchr(mode, '+')) flags |= O_WRONLY;
  int fd = open(name, flags);
  if (fd >= 0) {
    // TODO: use a buffer, unless file is known terminal.
    FILE *file = (FILE*)malloc(sizeof(FILE));
    memset(file, 0, sizeof(FILE));
    file->fd = fd;
    file->row = (flags == O_RDONLY);
    _LL_INSERT(&_stdio_files, file);
    return file;
  } else {
    return NULL;
  }
}
