#include <string.h>

size_t strxfrm(char *s1, const char *s2, size_t n) {
  return stpncpy(s1, s2, n) - s1;
}

size_t strxfrm_l(char *s1, const char *s2, size_t n, locale_t loc) {
  (void)(loc); //TODO: support locales
  return stpncpy(s1, s2, n) - s1;
}


