#include <stdint.h>
#include <coresrv/kbd.h>

#include <sys/keymap.h>

typedef struct kbd {
  keymap_t* keymap;
  int escape_next;
  int break_next;
  uint8_t keydown[KBD_MAX_KEYCODE];
} kbd_t;

extern kbd_t kbd;
