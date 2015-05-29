#include <stdio.h>
#include <limits.h>

int sprintf(char *str, const char* format, ...)
{
  va_list parameters;
  va_start(parameters, format);
  int bytes = vsnprintf(str, INT_MAX, format, parameters);
  va_end(parameters);
  return bytes;
}

int vsprintf(char *str, const char *format, va_list parameters) {
  return vsnprintf(str, INT_MAX, format, parameters);
}

int vsnprintf(char *str, size_t size, const char *format, va_list parameters) {
  FILE fake = (FILE){ .buffer=str, .buf_size=size-1, 
                      .buf_bytes=0, .fd=-1, .row=0 };
  int bytes = vfprintf(&fake, format, parameters);
  if (bytes >= 0 && (unsigned)bytes < size) {
    str[bytes] = 0;
  } else {
    str[size-1] = 0;
  }
  return bytes;
}
