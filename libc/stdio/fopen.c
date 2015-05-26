#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

FILE *fopen(const char *name, const char *mode) {
  (void)(mode); //TODO: mode; support create.
  int fd = open(name, 0);
  if (fd >= 0) {
    FILE *file = (FILE*)malloc(sizeof(FILE));
    memset(file, 0, sizeof(FILE));
    file->fd = fd;
    return file;
  } else {
    return NULL;
  }
}
