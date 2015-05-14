#include <stdio.h>
#include <hellbender.h>
#include <kernel/io.h>
#include <coresrv/kbd.h>

#include "kbd_impl.h"


void kbd_isr() {
  unsigned key = inb(0x60);
  for (int i = 0; key == 0 && i < 10; ++i) {
    iowait();
    key = inb(0x60);
  }
  
  switch (key) {
  case 0:
    // spurious interrupt?
    break;
  case 0xE0:
    kbd.escape_next = 1;
    break;
  case 0xF0:
    kbd.break_next = 1;
    break;
  case 0xFA:
    // ACK
    break;
  default:
    if (kbd.escape_next) {
      kbd.escape_next = 0;
      key |= 0x80;
    }

    unsigned flags = 0;
    if (kbd.keydown[KBD_KEY_LSHIFT]) flags |= KBD_FLAG_LSHIFT;
    if (kbd.keydown[KBD_KEY_RSHIFT]) flags |= KBD_FLAG_RSHIFT;
    if (kbd.keydown[KBD_KEY_LCTRL]) flags |= KBD_FLAG_LCTRL;
    if (kbd.keydown[KBD_KEY_RCTRL]) flags |= KBD_FLAG_RCTRL;
    if (kbd.keydown[KBD_KEY_LALT]) flags |= KBD_FLAG_LALT;
    if (kbd.keydown[KBD_KEY_RALT]) flags |= KBD_FLAG_RALT;

    unsigned event_type = 0;
    if (kbd.break_next) {
      event_type = KBD_EVENT_KEYUP;
      kbd.break_next = 0;
      kbd.keydown[key] = 0;
      
    } else {
      if (kbd.keydown[key]) event_type = KBD_EVENT_REPEAT;
      else event_type = KBD_EVENT_KEYDOWN;
      kbd.keydown[key] = 1;
    }

    unsigned event_idx = (kbd.last_event + 1) % KBD_MAX_EVENTS;
    if (event_idx != kbd.first_event) {
      kbd_event_t *event = &kbd.events[kbd.last_event];
      event->event_type = event_type;
      event->key_code = key;
      event->flags = flags;
      BARRIER;
      kbd.last_event = event_idx;
    } else {
      // TODO: beep
    }
  }
  syscall_iret();
}
