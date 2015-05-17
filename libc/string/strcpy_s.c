#include <string.h>
#include <errno.h>

errno_t strcpy_s(char* destination, size_t dest_size, const char* source) {
  if (destination == NULL) return EINVAL;
  unsigned i = 0;
  --dest_size; // leave room for terminator.
  for (; i < dest_size && source[i]; ++i) destination[i] = source[i];
  destination[i] = 0;
  return i == dest_size && source[i] ? ERANGE : 0;
}

