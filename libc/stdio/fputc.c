#include <stdio.h>
#include <stdlib.h>

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
  if (file == NULL || file->buffer == NULL) {
    char str[2] = { (char)ic, '\0' };
    syscall_print(str);
  } else {
    if (file->buf_bytes == BUFSIZ) fflush(file);
    file->buffer[(file->buf_bytes)++] = (char)ic;
    if (ic == '\n') fflush(file);
  }
  return ic;
}
#endif

