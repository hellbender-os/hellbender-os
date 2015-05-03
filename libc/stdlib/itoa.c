#include <stdlib.h>

inline void swap(char *x, char *y) {
  /*  *x = *x ^ *y;
  *y = *x ^ *y;
  *x = *x ^ *y;
  */
  char tmp = *x;
  *x = *y;
  *y = tmp;
}

char* itoa(int value, char* str, int base) {
  char *retval = str;
  if (value < 0) {
    (*str++) = '-';
    value = -value;
  }
  utoa(value, str, base);
  return retval;
}
