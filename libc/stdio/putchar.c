#include <stdio.h>
#include <stdlib.h>

int putchar(int ic) {
#if defined(__is_hellbender_kernel)
  return fputc(ic, NULL);
#else
  return fputc(ic, stdout);
#endif
}
