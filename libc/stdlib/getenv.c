#include <stdlib.h>
#include <string.h>

extern char **environ;

char* getenv (const char* name) {
  size_t len = strlen(name);
  for (char **ptr = environ; *ptr; ++ptr) {
    char *evar = *ptr;
    if (strncmp(evar, name, len) == 0
        && evar[len] == '=') {
      return evar + len + 1;
    }
  }
  return NULL;
}

