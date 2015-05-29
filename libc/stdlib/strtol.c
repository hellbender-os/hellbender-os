#include <stdlib.h>
#include <ctype.h>
#include <errno.h>

unsigned long long strtoX(const char *str, char **endptr, int base,
                          long long min, unsigned long long max) {
  // skip any leading white space:
  while (*str && isspace(*str)) ++str;

  // check optional sign character:
  int sign = 0;
  if (*str == '-') {
    sign = 1;
    str++;
  }

  // decode base:
  int strbase = 0;
  if (*str == '0') {
    if (tolower(str[1]) == 'x') {
      str += 2;
      strbase = 16;
    } else {
      str++;
      strbase = 8;
    }
  }
  if (!base) base = strbase;
  else if (base != strbase) {
    errno = EINVAL;
    return 0;
  }

  // decode value:
  unsigned long long val = 0;
  while (*str++) {
    char c = tolower(*str);
    int d = INT_MAX;
    if (c >= '0' && c <= '9') d = c - '0';
    else if (c >= 'a' && c <= 'z') d = c - 'a' + 10;
    if (d >= base) break;
    unsigned long long nval = val * base + d;
    if (nval < val) { // overflow
      errno = ERANGE;
      val = max;
      break;
    } else val = nval;
  }

  // check sign and limits:
  if (sign) {
    if (val > (unsigned long long)(-min)) {
      errno = ERANGE;
      val = (unsigned long long)min;
    } else {
      val = (unsigned long long)(-(long long)val);
    }
  } else if (val > max) {
    errno = ERANGE;
    val = max;
  }

  // mark last char and return final value:
  if (endptr) *endptr = (char*)str;
  return val;
}

unsigned long long strtoull(const char *str, char **endptr, int base) {
  return strtoX(str, endptr, base, 0, ULLONG_MAX);
}

unsigned long strtoul(const char *str, char **endptr, int base) {
  return (unsigned long)strtoX(str, endptr, base, 0, ULONG_MAX);
}

long long strtoll(const char *str, char **endptr, int base) {
  return (long long)strtoX(str, endptr, base, LLONG_MIN, LLONG_MAX);
}

long strtol(const char *str, char **endptr, int base) {
  return (long)strtoX(str, endptr, base, LONG_MIN, LONG_MAX);
}
