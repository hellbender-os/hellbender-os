#ifndef __CORESRV_DEV_CONSOLE_H__
#define __CORESRV_DEV_CONSOLE_H__

#include "cdev.h"

typedef struct console_op {
  int (*create)(vfs_node_t *node);
} console_op_t;

typedef void (*console_bind_t)(console_op_t *console_op);
#define CONSOLE_ID "coresrv::dev::console"
#define CONSOLE_BIND ((console_bind_t)BROKER_LOOKUP(CONSOLE_ID))

#endif
