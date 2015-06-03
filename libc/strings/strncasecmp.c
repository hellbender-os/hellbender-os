#include <strings.h>
#include <ctype.h>

int strncasecmp(const char *str1, const char *str2, size_t n) {
  while (n && *str1 && *str2 && tolower(*str1) == tolower(*str2)) {
    ++str1;
    ++str2;
    --n;
  }
  return tolower(*str1) - tolower(*str2);
}

int strncasecmp_l(const char *str1, const char *str2, size_t n, locale_t loc) {
  (void)(loc); // TODO: support locale.
  return strncasecmp(str1, str2, n);
}
