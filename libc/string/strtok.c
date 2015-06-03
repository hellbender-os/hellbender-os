#include <string.h>
#include <sys/threadlocal.h>

char *strtok(char *str, const char *delim) {
  return strtok_r(str, delim, &THREADLOCAL->strtok_saveptr);
}

char *strtok_r(char *str, const char *delim, char **saveptr) {
  if (str) *saveptr = str;
  char *ptr = *saveptr;
  if (!ptr) return NULL;
  ptr += strcspn(ptr, delim);
  char *start = ptr;
  ptr = strpbrk(ptr, delim);
  if (ptr) {
    *ptr = 0;
    *saveptr = ptr + 1;
  } else {
    *saveptr = NULL;
  }
  return *start ? start : NULL;
}
