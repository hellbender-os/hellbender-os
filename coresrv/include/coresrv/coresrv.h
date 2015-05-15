#ifndef _CORESRV_CORESRV_H
#define _CORESRV_CORESRV_H

#include <stdint.h>
#include <sys/types.h>

#include <kernel/idc.h>

typedef struct vfo vfo_t;
#include <coresrv/coresrv_idc.h>

// calls the default coresrv implementation of an IDC interface.
#define CORE_IDC(IFACE, ...) __IDC__ ## IFACE (__IDE__ ## IFACE, ##__VA_ARGS__)

#endif
