#include <string.h>
#include "fcntl_impl.h"

struct fcntl_data _fcntl_data;

void fcntl_init() {
  memset(&_fcntl_data, 0, sizeof(_fcntl_data));
}
