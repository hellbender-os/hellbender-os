#ifndef __CORESRV_BROKER_H__
#define __CORESRV_BROKER_H__

#include <hellbender/service_config.h>

#include <stdint.h>

typedef void* (*broker_lookup_t)(const char* service_id);

typedef void* (*broker_register_t)(const char* service_id, void* service_func);

#define BROKER_LOOKUP ((broker_lookup_t)(SERVICE_TABLE_BASE+8))

#define BROKER_ID "coresrv::broker"
#define BROKER_REGISTER ((broker_register_t)BROKER_LOOKUP(BROKER_ID))

#define REGISTER_SERVICE(func) (typeof(&func))syscall_register_service(func)

#endif
