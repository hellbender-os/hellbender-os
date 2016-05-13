#ifndef __CORESRV_DEV_INPUT_H__
#define __CORESRV_DEV_INPUT_H__

#define INPUT_EVENT_KEYDOWN 1
#define INPUT_EVENT_KEYUP   2

typedef struct __attribute__((packed)) input_event {
  union {
    struct {
      uint32_t relative_time;
      uint8_t type;
      uint8_t key_code;
      uint16_t flags;
    } keyboard;
    struct {
      uint32_t relative_time;
      uint8_t delta_x;
      uint8_t delta_y;
      uint16_t buttons;
    } mouse;
  };
} input_event_t;

#endif
