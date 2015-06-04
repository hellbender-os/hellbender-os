#include <stdio.h>
#include <stdarg.h>
#include <string.h>

int sscanf(const char *str, const char *format, ...) {
  va_list parameters;
  va_start(parameters, format);
  int bytes = vsscanf(str, format, parameters);
  va_end(parameters);
  return bytes;
}

int vsscanf(const char *str, const char *format, va_list parameters) {
  FILE fake = (FILE){ .buffer=(char*)str, .buf_size=strlen(str)+1, 
                      .buf_bytes=0, .buf_offset=0, .fd=-1, .row=1 };
  return vfscanf(&fake, format, parameters);
}

