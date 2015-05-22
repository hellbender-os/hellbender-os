#include <string.h>
#include <sys/keymap.h>

#include "kbd_impl.h"

kbd_t kbd;
keymap_t* keymap;

void kbd_init() {
  memset(&kbd, 0, sizeof(kbd));
  keymap = keymap_create("fi");
}
