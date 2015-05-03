#include <stdlib.h>

char* itoa(int value, char* str, int base) {
  char *retval = str;
  if (value < 0) {
    (*str++) = '-';
    value = -value;
  }
  _utoa(value, str, base);
  return retval;
}
