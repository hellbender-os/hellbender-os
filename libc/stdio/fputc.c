#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#if defined(__is_hellbender_kernel)
#include <kernel/tty.h>

int fputc(int ic, FILE* file) {
  (void)(file);
  terminal_putchar(ic);
  return ic;
}

#else
#include <hellbender.h>

int fputc(int ic, FILE *file) {
  if (file == NULL) { // assume libC is not yet set up.
    syscall_print((const char*)&ic, 1);

  } else if (file->buffer == NULL && file->fd >= 0) { // unbuffered
    write(file->fd, &ic, 1);
    
  } else { // buffered
    if (file->buf_bytes == BUFSIZ) fflush(file);
    if (file->buf_bytes < file->buf_size)
      file->buffer[(file->buf_bytes)++] = (char)ic;
    if (ic == '\n') fflush(file);
  }
  return ic;
}

#endif

