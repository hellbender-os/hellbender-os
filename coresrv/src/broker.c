#include <hellbender/broker.h>
#include <hellbender/syscall.h>
#include <hellbender/dict.h>
#include <hellbender/guard.h>

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

struct service_item {
  dict_item_t item;
  void* func;
}; // followed by null terminated service id.

struct broker {
  pthread_rwlock_t rwlock;
  dict_t services;
};

static struct broker broker;

void* broker_lookup(const char* service_id) {
  GUARD_PTHREAD_RWLOCK_READ(&broker.rwlock);
  
  dict_item_t *ptr = dict_find(&broker.services, service_id, strlen(service_id));
  if (ptr) {
    struct service_item *si = dict_container(ptr, struct service_item, item);
    return si->func;
  }
  return 0;
}

void* broker_register(const char* service_id, void* service_func) {
  GUARD_PTHREAD_RWLOCK_WRITE(&broker.rwlock);

  int key_len = strlen(service_id);
  struct service_item *si = malloc(sizeof(*si) + key_len + 1);
  char *key = ((char *)si) + sizeof(*si);
  strcpy(key, service_id);
  si->func = service_func;
  si->item.key = key;
  si->item.key_len = key_len;
  dict_item_t *old = dict_replace(&broker.services, &si->item);
  if (old) {
    struct service_item *oi = rbtree_container(old, struct service_item, item);
    void *old_func = oi->func;
    free(oi);
    return old_func;
  } else return 0;
}

void broker_init() {
  broker.rwlock = (pthread_rwlock_t)PTHREAD_RWLOCK_INITIALIZER;
  dict_init(&broker.services);

  uintptr_t lookup = syscall_register_service(broker_lookup);
  if (lookup != SERVICE_TABLE_BASE+8) exit(-1);
  broker_register(BROKER_ID, REGISTER_SERVICE(broker_register));
}
