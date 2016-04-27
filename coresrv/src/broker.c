#include "hellbender/config.h"
#include "hellbender/list.h"
#include "hellbender/syscall.h"

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

#define BROKER_SERVICE_ID_LEN 32

struct service_item {
  list_item_t item;
  char name[BROKER_SERVICE_ID_LEN];
  void* func;
};

struct broker {
  union {
    struct {
      pthread_mutex_t mutex;
      list_t services;
    };
    uint8_t dummy[CACHE_LINE];
  };
};

static struct broker broker;

void* broker_resolve(const char* serviceId) {
  pthread_mutex_lock(&broker.mutex);

  void *func = 0;
  for (struct list_item *item = list_first(&broker.services);
       item; item = list_next(item)) {
    struct service_item *si = list_container(item, struct service_item, item);
    if (strncmp(si->name, serviceId, BROKER_SERVICE_ID_LEN) == 0) {
      func = si->func;
      break;
    }
  }

  pthread_mutex_lock(&broker.mutex);
  return func;
}

void* broker_bind(const char* serviceId, uintptr_t service_func) {
  pthread_mutex_lock(&broker.mutex);

  // TODO: remove old if exists.
  void *old_func = 0;

  struct service_item *si = 0;//malloc(sizeof(struct service_item));
  strncpy(si->name, serviceId, BROKER_SERVICE_ID_LEN);
  si->func = (void*)service_func;
  list_insert(&broker.services, &si->item);

  pthread_mutex_unlock(&broker.mutex); 
  return old_func;
}

void broker_init() {
  broker.mutex = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;

  uintptr_t resolver = syscall_register_service(broker_resolve);
  if (resolver != SERVICE_TABLE_BASE+8) exit(-1);
  //broker_bind("broker::bind", syscall_register_service(broker_bind));
}
