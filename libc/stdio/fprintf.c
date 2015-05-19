#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void print(FILE *file, const char* data, size_t data_length)
{
#if !defined(__is_hellbender_kernel)
  if (file == NULL || file->buffer == NULL) {
#endif
    for (size_t i = 0; i < data_length; i++) {
      putchar((int) ((const unsigned char*) data)[i]);
    }
#if !defined(__is_hellbender_kernel)
  } else {
    for (size_t i = 0; i < data_length; i++) {
      if (file->buf_bytes == BUFSIZ) fflush(file);
      file->buffer[(file->buf_bytes)++] = data[i];
      if (data[i] == '\n') fflush(file);
    }
  }
#endif
}

int fprintf(FILE *file, const char* restrict format, ...)
{
  va_list parameters;
  va_start(parameters, format);
  int bytes = vfprintf(file, format, parameters);
  va_end(parameters);
  return bytes;
}

int vfprintf(FILE* file, const char* restrict format, va_list parameters) {
  int written = 0;
  size_t amount;
  bool rejected_bad_specifier = false;

  while ( *format != '\0' ) {
    if ( *format != '%' ) {
    print_c:
      amount = 1;
      while ( format[amount] && format[amount] != '%' )
        amount++;
      print(file, format, amount);
      format += amount;
      written += amount;
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
      print(file, &c, sizeof(c));
    }
    else if ( *format == 's' ) {
      format++;
      const char* s = va_arg(parameters, const char*);
      print(file, s, strlen(s));
    }
    else if ( *format == 'd' || *format == 'i') {
      format++;
      int i = va_arg(parameters, int);
      char s[16];
      _itoa(i, s, 10);
      print(file, s, strlen(s));
    }
    else if ( *format == 'u') {
      format++;
      unsigned i = va_arg(parameters, unsigned);
      char s[16];
      _utoa(i, s, 10);
      print(file, s, strlen(s));
    }
    else if ( *format == 'x') {
      format++;
      unsigned i = va_arg(parameters, unsigned);
      char s[10];
      _utoa(i, s, 16);
      print(file, s, strlen(s));
    }
    else {
      goto incomprehensible_conversion;
    }
  }

  return written;
}
