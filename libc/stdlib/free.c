#include <stdlib.h>
#include <stdint.h>
#include <sys/mman.h>
#include "heap.h"

void free(void *ptr) {
  _heap_free(ptr);
}
