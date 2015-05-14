#ifndef _CORESRV_PS2_H
#define _CORESRV_PS2_H

#define PS2_MODE_KEYBOARD 1
#define PS2_MODE_STDMOUSE 2
#define PS2_MODE_WHEELMOUSE 3
#define PS2_MODE_5BUTMOUSE 4

typedef struct ps2 {
  unsigned chan1_mode;
  unsigned chan2_mode;
} ps2_t;

extern ps2_t ps2;

#endif
