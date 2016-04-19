#include <string.h>

size_t strcspn(const char *str, const char *chrs) {
  const char *ptr = str;
  for (; *ptr; ++ptr) {
    for (const char *tst = chrs; *tst; ++tst) {
      if (*ptr == *tst) return ptr - str;
    }
  }
  return ptr - str;
}
