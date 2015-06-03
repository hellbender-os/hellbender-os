#include <strings.h>
#include <ctype.h>

int strcasecmp(const char *str1, const char *str2) {
  while (*str1 && *str2 && tolower(*str1) == tolower(*str2)) ++str1, ++str2;
  return tolower(*str1) - tolower(*str2);
}

int strcasecmp_l(const char *str1, const char *str2, locale_t loc) {
  (void)(loc); // TODO: support locales.
  return strcasecmp(str1, str2);
}

