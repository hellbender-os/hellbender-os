#include <stdint.h>
#include <coresrv/kbd.h>

#define KBD_MAX_EVENTS 16

typedef struct kbd {
  int escape_next;
  int break_next;
  uint8_t keydown[KBD_MAX_KEYCODE];
  kbd_event_t events[KBD_MAX_EVENTS];
  unsigned first_event;
  volatile unsigned last_event;
} kbd_t;

extern kbd_t kbd;
