#include <stdio.h>

int sprintf(char *str, const char* format, ...)
{
  va_list parameters;
  va_start(parameters, format);
  FILE fake = { str, 0, -1, 0 };
  int bytes = vfprintf(&fake, format, parameters);
  str[bytes] = 0;
  va_end(parameters);
  return bytes;
}
