#include <string.h>

int strcoll(const char *s1, const char *s2) {
  return strcmp(s1, s2);
}

int strcoll_l(const char *s1, const char *s2, locale_t loc) {
  (void)(loc); //TODO: support locales.
  return strcmp(s1, s2);
}
