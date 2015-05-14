#include <string.h>
#include <sys/keymap.h>

extern keymap_t keymap_fi;

keymap_t* keymap_create(const char* locale) {
  keymap_t* km = NULL;
  if (strcmp("fi", locale) == 0) {
    km = &keymap_fi;
  } else return NULL;

  for (unsigned i = 0; i < KBD_MAX_KEYCODE; ++i) {
    if (i != KBD_KEY_SPACE) {
      if (km->char_plain[i] == ' ') km->char_plain[i] = 0;
      if (km->char_shift[i] == ' ') km->char_shift[i] = 0;
      if (km->char_altgr[i] == ' ') km->char_altgr[i] = 0;
    }
  }
  return km;
}
