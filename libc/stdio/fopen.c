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
    FILE *file = calloc(1, sizeof(*file));
    file->fd = fd;
    file->row = (flags == O_RDONLY);
    if (!isatty(fd)) {
      file->buf_size = _FILE_BUFSIZE;
      file->buffer = calloc(1, file->buf_size);
    }
    list_insert(&_stdio.files, &file->item);
    return file;
  } else {
    return NULL;
  }
}
