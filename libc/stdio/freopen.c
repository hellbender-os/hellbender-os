#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

FILE *freopen(const char *name, const char *mode, FILE *file) {
  (void)(mode); //TODO: support mode flags.
  if (name) {
    fclose(file);
    int fd = open(name, 0);
    if (fd >= 0) {
      // TODO: use a buffer, unless file is known terminal.
      memset(file, 0, sizeof(FILE));
      file->fd = fd;
      return file;
    } else {
      return NULL;
    }
  } else {
    // TODO change mode.
    return file;
  }
}
