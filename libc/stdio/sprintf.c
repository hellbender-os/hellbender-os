#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char* print(char *str, const char* data, size_t data_length)
{
  if (str)
    for ( size_t i = 0; i < data_length; i++, str++, data++ )
      *str = *data;
  return str;
}

int sprintf(char *str, const char* restrict format, ...)
{
  va_list parameters;
  va_start(parameters, format);

  int written = 0;
  size_t amount;
  bool rejected_bad_specifier = false;

  while ( *format != '\0' )
    {
      if ( *format != '%' )
        {
        print_c:
          amount = 1;
          while ( format[amount] && format[amount] != '%' )
            amount++;
          str = print(str, format, amount);
          format += amount;
          written += amount;
          continue;
        }

      const char* format_begun_at = format;

      if ( *(++format) == '%' )
        goto print_c;

      if ( rejected_bad_specifier )
        {
        incomprehensible_conversion:
          rejected_bad_specifier = true;
          format = format_begun_at;
          goto print_c;
        }

      if ( *format == 'c' )
        {
          format++;
          char c = (char) va_arg(parameters, int /* char promotes to int */);
          str = print(str, &c, sizeof(c));
        }
      else if ( *format == 's' )
        {
          format++;
          const char* s = va_arg(parameters, const char*);
          str = print(str, s, strlen(s));
        }
      else if ( *format == 'd' || *format == 'i')
        {
          format++;
          int i = va_arg(parameters, int);
          char s[16];
          itoa(i, s, 10);
          str = print(str, s, strlen(s));
        }
      else if ( *format == 'u')
        {
          format++;
          unsigned i = va_arg(parameters, unsigned);
          char s[16];
          _utoa(i, s, 10);
          str = print(str, s, strlen(s));
        }
      else if ( *format == 'x')
        {
          format++;
          unsigned i = va_arg(parameters, unsigned);
          char s[10];
          _utoa(i, s, 16);
          str = print(str, s, strlen(s));
        }
      else
        {
          goto incomprehensible_conversion;
        }
    }

  va_end(parameters);

  if (str) *str = '\0';
  return written;
}
