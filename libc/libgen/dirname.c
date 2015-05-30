#include <libgen.h>
#include <string.h>

static const char dot[] = ".";

char *dirname(char *path) {
  if (path == NULL) return (char*)dot;
  char *slash = strrchr(path, '/');
  if (!slash) return (char*)dot;
  while (slash > path && *slash == '/') *slash-- = 0;
  return path;
}
