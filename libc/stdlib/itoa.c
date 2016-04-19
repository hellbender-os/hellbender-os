#include <stdlib.h>

char* _itoa_n(int value, char* str, size_t size, int base) {
  if (value < 0) {
    str = _utoa_n(-value, str, size, base);
    (*--str) = '-';
  } else {
    str = _utoa_n(value, str, size, base);
  }
  return str;
}
