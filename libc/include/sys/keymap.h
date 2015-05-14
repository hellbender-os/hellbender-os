#ifndef _SYS_KEYMAP_H
#define _SYS_KEYMAP_H

#include <coresrv/kbd.h>

typedef struct keymap {
  char char_plain[KBD_MAX_KEYCODE];
  char char_shift[KBD_MAX_KEYCODE];
  char char_altgr[KBD_MAX_KEYCODE];
} keymap_t;

keymap_t* keymap_create(const char* locale);
int keymap_code2char(keymap_t* keymap, unsigned key_code, unsigned flags);

#endif
