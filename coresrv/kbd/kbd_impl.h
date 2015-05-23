#include <stdint.h>
#include <semaphore.h>
#include <coresrv/kbd.h>

#define KBD_MAX_EVENTS 32

typedef struct kbd {
  int escape_next;
  int break_next;
  uint8_t keydown[KBD_MAX_KEYCODE];
  kbd_event_t events[KBD_MAX_EVENTS];
  unsigned first_event;
  volatile unsigned last_event;

  sem_t event_sema; // posted for every event, waited for every read.
} kbd_t;

extern kbd_t kbd;
