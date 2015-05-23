#include <stdint.h>
#include <hellbender.h>
#include <coresrv/kbd.h>

#include "kbd_impl.h"

__IDCIMPL__ int coresrv_kbd_getc(IDC_PTR) {
  while (kbd.first_event != kbd.last_event) {
    sem_wait(&kbd.event_sema);
    unsigned event_idx = kbd.first_event;
    kbd_event_t event = kbd.events[event_idx];
    BARRIER;
    kbd.first_event = (event_idx + 1) % KBD_MAX_EVENTS;
    if (event.event_type & KBD_EVENT_KEYDOWN) {
      return (int)(event.key_code | event.flags << 16);
    }
  }
  return -1;
}

