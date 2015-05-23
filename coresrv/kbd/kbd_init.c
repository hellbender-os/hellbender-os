#include <string.h>
#include <sys/keymap.h>

#include "kbd_impl.h"

kbd_t kbd;
keymap_t* keymap;

void kbd_init() {
  memset(&kbd, 0, sizeof(kbd));
  sem_init(&kbd.event_sema, 1, 0);
  keymap = keymap_create("fi");
}
