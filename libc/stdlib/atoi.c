#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

int atoi(const char *str) {
  // skip any leading white space:
  while (*str && isspace(*str)) ++str;

  // check optional sign character:
  int sign = 0;
  if (*str == '-') {
    sign = 1;
    str++;
  }
  
  // decode value:
  unsigned val = 0;
  while (*str) {
    char c = *str++;
    int d = INT_MAX;
    if (c >= '0' && c <= '9') d = c - '0';
    else break;
    unsigned nval = val * 10 + d;
    if (nval < val || nval > INT_MAX) { // overflow
      val = INT_MAX;
      break;
    } else val = nval;
  }

  // check sign and limits:
  if (sign) return -(int)val;
  else return (int)val;
}

