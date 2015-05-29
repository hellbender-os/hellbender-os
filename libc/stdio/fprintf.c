#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <hellbender.h>

#if defined(__is_hellbender_kernel)

static size_t print(FILE *file, const char* data, size_t data_length) {
  (void)(file);
  for (size_t i = 0; i < data_length; i++) {
    putchar((int) ((const unsigned char*) data)[i]);
  }
  return data_length;
}

#else

static size_t print(FILE *file, const char* data, size_t data_length) {
  if (file == NULL) { // assume libC is not yet set up.
    syscall_print(data, data_length);

  } else if (file->buffer == NULL && file->fd >= 0) { // unbuffered
    write(file->fd, data, data_length);
    
  } else { // buffered
    for (size_t i = 0; i < data_length; i++) {
      if (file->buf_bytes == file->buf_size) fflush(file);
      if (file->buf_bytes < file->buf_size)
        file->buffer[(file->buf_bytes)++] = data[i];
      if (data[i] == '\n') fflush(file);
    }
  }
  return data_length;
}

#endif

int fprintf(FILE *file, const char* format, ...)
{
  va_list parameters;
  va_start(parameters, format);
  int bytes = vfprintf(file, format, parameters);
  va_end(parameters);
  return bytes;
}

int vfprintf(FILE* file, const char* format, va_list parameters) {
  int written = 0;
  size_t amount;
  bool rejected_bad_specifier = false;

  while ( *format != '\0' ) {
    if ( *format != '%' ) {
    print_c:
      amount = 1;
      while ( format[amount] && format[amount] != '%' )
        amount++;
      written += print(file, format, amount);
      format += amount;
      continue;
    }

    const char* format_begun_at = format;

    if ( *(++format) == '%' )
      goto print_c;

    if ( rejected_bad_specifier ) {
    incomprehensible_conversion:
      rejected_bad_specifier = true;
      format = format_begun_at;
      goto print_c;
    }

    if ( *format == 'c' ) {
      format++;
      char c = (char) va_arg(parameters, int /* char promotes to int */);
      written += print(file, &c, sizeof(c));
    }
    else if ( *format == 's' ) {
      format++;
      const char* s = va_arg(parameters, const char*);
      written += print(file, s, strlen(s));
    }
    else if ( *format == 'd' || *format == 'i') {
      format++;
      int i = va_arg(parameters, int);
      char s[16];
      _itoa(i, s, 10);
      written += print(file, s, strlen(s));
    }
    else if ( *format == 'u') {
      format++;
      unsigned i = va_arg(parameters, unsigned);
      char s[16];
      _utoa(i, s, 10);
      written += print(file, s, strlen(s));
    }
    else if ( *format == 'x') {
      format++;
      unsigned i = va_arg(parameters, unsigned);
      char s[10];
      _utoa(i, s, 16);
      written += print(file, s, strlen(s));
    }
    else {
      goto incomprehensible_conversion;
    }
  }

  return written;
}
