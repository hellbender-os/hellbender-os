#include <string.h>
#include <errno.h>

errno_t strcat_s(char* destination, size_t dest_size, const char* source) {
  if (destination == NULL) return EINVAL;
  --dest_size; // leave room for terminator.
  size_t i = 0;
  while (i < dest_size && destination[i]) ++i; // skip to dest end.
  size_t j = 0;
  for (; i < dest_size && source[j]; ++i, ++j) destination[i] = source[j];
  destination[i] = 0;
  return i == dest_size && source[j] ? ERANGE : 0;
}

