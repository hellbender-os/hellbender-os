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

char* _utoa(unsigned value, char* str, int base) {
  const char digits[] = "0123456789abcdefghijklmnopqrstuvwxyz";
  char *retval = str;
  if (base >= 2 && base <= 36) { 
    if (value == 0) {
      (*str++) = '0';
    }
    if (value > 0) {
      char *begin = str;
      while (value > 0) {
	int digit = value % base;
	value /= base;
	(*str++) = digits[digit];
      }
      char *end = str - 1;
      while (begin < end) {
	swap(begin, end);
	++begin;
	--end;
      }
    }
  }
  *str = '\0';
  
  return retval;
}
