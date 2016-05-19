#include <stdio.h>
#include <stdarg.h>

int scanf(const char *format, ...) {
  va_list parameters;
  va_start(parameters, format);
  int bytes = vscanf(format, parameters);
  va_end(parameters);
  return bytes;
}

int vscanf(const char *format, va_list parameters) {
  return vfscanf(stdin, format, parameters);
}
