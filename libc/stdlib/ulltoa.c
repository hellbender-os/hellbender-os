#include <stdlib.h>

const char _ldigits[] = "0123456789abcdefghijklmnopqrstuvwxyz";
const char _udigits[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

char* _ulltoa_n(unsigned long long value, char* str, size_t size, int base) {
  const char* digits = _ldigits;
  if (base < 0) {
    base = -base;
    digits = _udigits;
  }
  str = str + size;
  *(--str) = '\0';
  if (base >= 2 && base <= 36) { 
    if (value == 0) {
      *(--str) = '0';
    }
    while (value > 0) {
      int digit = value % base;
      value /= base;
      *(--str) = digits[digit];
    }
  }
  return str;
}
