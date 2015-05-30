#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

int atoi(const char *str) {
  return (int)strtol(str, NULL, 10);
}

