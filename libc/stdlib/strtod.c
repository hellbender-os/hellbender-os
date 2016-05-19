#include <stdlib.h>
#include <float.h>
#include <math.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>

double strtod(const char *restrict nptr, char **restrict endptr) {
  long double ld = strtold(nptr, endptr);
  if (ld > DBL_MAX) { errno = ERANGE; return HUGE_VAL; }
  else if (ld < -DBL_MAX) { errno = ERANGE; return -HUGE_VAL; }
  else if (ld > -DBL_MIN && ld < DBL_MIN) { errno = ERANGE; return ld < 0 ? -0. : +0.; }
  return (double)ld;
}

float strtof(const char *restrict nptr, char **restrict endptr) {
  long double ld = strtold(nptr, endptr);
  if (ld > FLT_MAX) { errno = ERANGE; return HUGE_VALF; }
  else if (ld < -FLT_MAX) { errno = ERANGE; return -HUGE_VALF; }
  else if (ld > -FLT_MIN && ld < FLT_MIN) { errno = ERANGE; return ld < 0 ? -0. : +0.; }
  return (float)ld;
}

long double strtold(const char *restrict nptr, char **restrict endptr) {
  // number as sign * value
  int sign = 0;
  long double value = 0;

  // skip isspace(c)
  while (*nptr && isspace(*nptr)) ++nptr;

  // optional sign '+' or '-'
  if (*nptr == '+') ++nptr, sign = 1;
  if (*nptr == '-') ++nptr, sign = -1;

  // 'inf', 'infinity' (ignore case)
  if (strncasecmp(nptr, "inf", 3) == 0) {
    nptr += 3;
    value = sign < 0 ? -__builtin_infl() : __builtin_infl();
  } 
  else if (strncasecmp(nptr, "infinity", 8) == 0) {
    nptr += 8;
    value = sign < 0 ? -__builtin_infl() : __builtin_infl();
  }

  // 'nan', 'nan.*' (ignore case)
  else if (strncasecmp(nptr, "nan", 3) == 0) {
    nptr += 3;
    sign = 0;
    value = __builtin_nanl("");
  }

  else {
    // value as x * 2^p
    __uint128_t x = 0;
    int p = 0;

    // 0x or 0X for hex value
    if (strncasecmp(nptr, "0x", 2) == 0) {
      x = strtoull(nptr + 2, (char**)&nptr, 16);
      // optional radix '.'
      if (*nptr == '.') {
        ++nptr;
        const char* end;
        unsigned long long y = strtoull(nptr, (char**)&end, 16);
        if (y) for (; nptr < end; ++nptr) x <<= 4, p -= 4;
        x += y;
      }
      // optional binary exponent 'p'
      if (*nptr && (*nptr == 'p' || *nptr == 'P')) {
        ++nptr;
        long y = strtol(nptr, (char**)&nptr, 10);
        p += y;
      }
    } 
    
    // decimal value
    else {
      // 1st parse the string as a number x * 10^e:
      int e = 0;
      x = strtoull(nptr + 2, (char**)&nptr, 10);
      // optional radix '.'
      if (*nptr == '.') {
        ++nptr;
        const char* end;
        unsigned long long y = strtoull(nptr, (char**)&end, 16);
        if (y) for (; nptr < end; ++nptr) x *= 10, e -= 1;
        x += y;
      }
      // optional exponent 'e'
      if (*nptr && (*nptr == 'e' || *nptr == 'E')) {
        ++nptr;
        long y = strtol(nptr, (char**)&nptr, 10);
        e += y;
      }
      // then convert 10^e to 2^p
      __uint128_t mask = 0xffffffff00000000ull;
      mask <<= 64;
      if (x && e > 0) {
        while (e > 0) {
          --e, x *= 10; // x*10^e remains constant
          // keep the highest 32 bits clear, to avoid overflowing x.
          while (x & mask) x >>= 16, p += 16;
        }
      }
      else if (x && e < 0) {
        // quickly clear out the lowest most digits (as we have 128 bits to spare)
        x <<= 48, p -= 48;
        while (e < 0) {
          // keep the higest 32 bits non-clear, to avoid underflowing x.
          while ((x & mask) == 0) x <<= 16, p -= 16;
          ++e, x /= 10; // x*10^e remains constant
        }
      }
    }

    // convert x*2^p to long double value
    __uint128_t mask = 1;
    mask <<= LDBL_MANT_DIG;
    mask = -mask;
    while ((x>>16) & mask) x >>= 16, p += 16;
    while ((x>>4) & mask) x >>= 4, p += 4;
    while (x & mask) x >>= 1, ++p;
    value = ldexpl((long double)x, p);
  }

  if (endptr) *endptr = (char*)nptr;
  return sign < 0 ? -value : value;
}

