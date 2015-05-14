#include <stdint.h>
#include <hellbender.h>
#include <coresrv/kbd.h>

#include "kbd_impl.h"

int _idc_coresrv_kbd_getc(__attribute__((unused))uintptr_t retval) {
  while (kbd.first_event != kbd.last_event) {
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

