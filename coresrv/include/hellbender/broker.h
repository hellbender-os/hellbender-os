#ifndef __CORESRV_BROKER_H__
#define __CORESRV_BROKER_H__

#include <hellbender/service_config.h>

typedef void* (*broker_resolve_t)(const char* serviceId);

typedef void* (*broker_bind_t)(const char* serviceId, uintptr_t service_handle);

#define broker_resolve ((broker_resolve_t)SERVICE_TABLE_BASE+8);


#endif
