#include <string.h>

char* strcat(char* destination, const char* source) {
  unsigned i = 0;
  while (destination[i]) ++i; // skip to dest end.
  unsigned j = 0;
  for (; source[j]; ++i, ++j) destination[i] = source[j];
  destination[i] = 0;
  return destination;
}

