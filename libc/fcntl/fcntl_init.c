#include <string.h>
#include "fcntl_impl.h"

struct fcntl_data fcntl_data;

void fcntl_init() {
  memset(&fcntl_data, 0, sizeof(fcntl_data));
}