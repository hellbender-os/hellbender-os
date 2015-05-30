#include <libgen.h>
#include <string.h>

static const char dot[] = ".";

char *basename(char *path) {
  if (!path || !*path) return (char*)dot;
  char *slash = strrchr(path, '/');
  if (!slash) return (char*)dot;
  while (1) {
    if (slash > path && !slash[1]) {
      *slash = 0;
      slash = strrchr(path, '/');
      continue;
    }
    else if (!slash[1]) return slash;
    return slash + 1;
  }
}
