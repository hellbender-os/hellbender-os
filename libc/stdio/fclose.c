#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int fclose(FILE *file) {
  if (close(file->fd) == 0) {
    free(file);
    _LL_REMOVE(file);
    return 0;
  } else {
    free(file);
    return EOF;
  }
}
