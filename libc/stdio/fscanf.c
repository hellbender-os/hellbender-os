#include <stdio.h>
#include <stdarg.h>

int fscanf(FILE *file, const char *format, ...) {
  va_list parameters;
  va_start(parameters, format);
  int bytes = vfscanf(file, format, parameters);
  va_end(parameters);
  return bytes;
}

int vfscanf(FILE *file, const char *format, va_list parameters) {
  (void)(file);
  (void)(format);
  (void)(parameters);
  return 0; //TODO
}
