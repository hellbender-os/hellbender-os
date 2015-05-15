#include <string.h>

char* strcpy(char* destination, const char* source) {
  int i = 0;
  for (; source[i]; ++i) destination[i] = source[i];
  destination[i] = 0;
  return destination;
}

