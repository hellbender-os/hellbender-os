#include <string.h>
#include <ctype.h>

int strcasecmp(const char *s1, const char *s2) {
  while (*s1 && *s2 && tolower(*s1) == tolower(*s2)) ++s1, ++s2;
  return (int)tolower(*s1) - (int)tolower(*s2);
}

int strncasecmp(const char *s1, const char *s2, size_t n) {
  while (n && *s1 && *s2 && tolower(*s1) == tolower(*s2)) ++s1, ++s2, --n;
  return n ? (int)tolower(*s1) - (int)tolower(*s2) : 0;
}
