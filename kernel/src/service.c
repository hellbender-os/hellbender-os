#include "service.h"
#include "process.h"
#include "cpu.h"

uint64_t service_table[SERVICE_TABLE_SIZE/8] __attribute__((aligned(PAGE_SIZE)));

struct service {
  unsigned next_handle;
};

static struct service service;

uint64_t service_register(uint64_t target) {
  if (service.next_handle + 8 < SERVICE_TABLE_SIZE) {
    service.next_handle += 8;
    service_table[service.next_handle/8] = target;
    return SERVICE_TABLE_BASE + service.next_handle;
  } else return 0;
}

void service_call(uint64_t handle) {
  if (!cpu.current_thread->in_service) {
    cpu.current_thread->in_service = 1;
    // TODO: disable user mode exec.
    // TODO: enable service mode.
    // TODO: set PCID
  }
  CPU_THREAD_STATE->context.rip = service_table[handle/8];
}

void service_return() {
    cpu.current_thread->in_service = 0;
    // TODO: enable user mode exec.
    // TODO: disable service mode.
    // TODO: set PCID
}
