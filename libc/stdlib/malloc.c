#include <stdlib.h>
#include <stdio.h>

void* malloc(size_t size) {
#if defined(__is_hellbender_kernel)
  return NULL;
#else
  return NULL;
#endif
}
