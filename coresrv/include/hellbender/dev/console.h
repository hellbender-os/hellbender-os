#ifndef __CORESRV_DEV_CONSOLE_H__
#define __CORESRV_DEV_CONSOLE_H__

#include "cdev.h"

typedef struct console {
  int (*create)(cdev_t *dev);
} console_t;

typedef void (*console_bind_t)(console_t *vfs);
#define CONSOLE_ID "coresrv::dev::console"
#define CONSOLE_BIND ((console_bind_t)BROKER_LOOKUP(CONSOLE_ID))

#endif
