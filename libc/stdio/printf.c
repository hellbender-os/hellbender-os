#include <stdio.h>
#include <stdlib.h>

int printf(const char* format, ...)
{
  va_list parameters;
  va_start(parameters, format);
#if defined(__is_hellbender_kernel)
  int bytes = vfprintf(NULL, format, parameters);
#else
  int bytes = vfprintf(stdout, format, parameters);
#endif
  va_end(parameters);
  return bytes;
}
