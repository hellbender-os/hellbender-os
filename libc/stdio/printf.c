#include <stdio.h>
#include <stdlib.h>

int printf(const char* format, ...)
{
  va_list parameters;
  va_start(parameters, format);
  int bytes = vfprintf(stdout, format, parameters);
  va_end(parameters);
  return bytes;
}

int vprintf(const char *restrict format, va_list ap) {
  return vfprintf(stdout, format, ap);
}
