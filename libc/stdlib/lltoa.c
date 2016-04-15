#include <stdlib.h>

char* _lltoa_n(long long value, char* str, size_t size, int base) {
  if (value < 0) {
    str = _ulltoa_n(-value, str, size, base);
    (*--str) = '-';
  } else {
    str = _ulltoa_n(value, str, size, base);
  }
  return str;
}
