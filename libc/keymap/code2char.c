#include <sys/keymap.h>

int keymap_code2char(keymap_t *keymap, unsigned key_code, unsigned flags) {
  if (key_code >= KBD_MAX_KEYCODE) return -1;
  
  int shift = flags & KBD_FLAG_SHIFT && !(flags & ~KBD_FLAG_SHIFT);
  int altgr = flags & KBD_FLAG_RALT && !(flags & ~KBD_FLAG_RALT);
  if (!flags) {
    return keymap->char_plain[key_code];
  }
  if (shift) {
    return keymap->char_shift[key_code];
  }
  if (altgr) {
    return keymap->char_altgr[key_code];
  }
  return -1;
}

