#include <string.h>
#include "kbd_impl.h"

kbd_t kbd;

void kbd_init() {
  memset(&kbd, 0, sizeof(kbd));
}
