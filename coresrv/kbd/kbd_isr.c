#include <stdio.h>
#include <sys/keymap.h>
#include <hellbender.h>
#include <kernel/io.h>
#include <coresrv/kbd.h>
#include <coresrv/rtc.h>
#include <coresrv/tty.h>

#include "kbd_impl.h"

static int magic_key(unsigned event_type, unsigned key, unsigned flags) {
  if (flags == (KBD_FLAG_LSHIFT+KBD_FLAG_LCTRL+KBD_FLAG_LALT)) {
    if (event_type == KBD_EVENT_KEYDOWN) {
      int c = keymap_code2char(keymap, key, 0);
      switch (c) {
      case '1': // switch between virtual terminals.
      case '2':
      case '3':
        {
          unsigned tty_id = c - '1';
          dev_tty_switch_to(tty_id);
        }
        break;
      }
    }
    return 1;
  } else return 0;
}

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

    if (magic_key(event_type, key, flags)) break;

    unsigned event_idx = (kbd.last_event + 1) % KBD_MAX_EVENTS;
    if (event_idx != kbd.first_event) {
      kbd_event_t *event = &kbd.events[kbd.last_event];
      event->event_type = event_type;
      event->key_code = key;
      event->flags = flags;
      BARRIER;
      kbd.last_event = event_idx;
    } else {
      coresrv_rtc_beep(NO_IDC, 800, 4);
    }
  }
  syscall_iret();
}
