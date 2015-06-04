#ifndef _CORESRV_KBD_H
#define _CORESRV_KBD_H

#include <coresrv/coresrv.h>

#define KBD_MAX_KEYCODE 256

#define KBD_EVENT_KEYDOWN 1
#define KBD_EVENT_KEYUP   2
#define KBD_EVENT_REPEAT  5

#define KBD_KEY_LSHIFT 0x12
#define KBD_KEY_RSHIFT 0x59
#define KBD_KEY_LCTRL  0x14
#define KBD_KEY_RCTRL  0x94
#define KBD_KEY_LALT   0x11
#define KBD_KEY_RALT   0x91

#define KBD_KEY_SPACE 0x29

#define KBD_FLAG_LSHIFT 1
#define KBD_FLAG_RSHIFT 2
#define KBD_FLAG_SHIFT  3

#define KBD_FLAG_LCTRL 4
#define KBD_FLAG_RCTRL 8
#define KBD_FLAG_CTRL  12

#define KBD_FLAG_LALT  16
#define KBD_FLAG_RALT  32
#define KBD_FLAG_ALT   48

typedef struct kbd_event {
  unsigned event_type;
  unsigned key_code;
  unsigned flags;
  int plain_c; // key interpret without flags.
  int real_c; // key interpret with flags.
} kbd_event_t;

#define KBD_MAKE_CODE(key, flag) (((key)&0xffff) | ((flags)&0xff) << 16)
#define KBD_GETC_KEYCODE(i) (i & 0xffff)
#define KBD_GETC_FLAGS(i) ((i>>16) & 0xff)

#endif
