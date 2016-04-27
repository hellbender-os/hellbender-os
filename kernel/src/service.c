#include "service.h"
#include "process.h"
#include "spin.h"
#include "cpu.h"
#include "page.h"

uint64_t service_table[SERVICE_TABLE_SIZE/8] __attribute__((aligned(PAGE_SIZE)));

struct service {
  union {
    struct {
      spinlock_raw_t lock;
      unsigned next_handle;
      uintptr_t next_service;
    };
    uint8_t dummy[CACHE_LINE];
  };
};

static struct service service;
static uint64_t service_pdpt[512] __attribute__((aligned(PAGE_SIZE)));

void service_init() {
  // map the service virtual memory area to correct location.
  page_set_pdpt((void*)SERVICE_OFFSET, kernel_v2p(service_pdpt), PAGE_USER_RW);
}

uintptr_t service_relocate(struct process_descriptor* pd) {
  SPIN_GUARD_RAW(service.lock);
  uintptr_t base = SERVICE_OFFSET + service.next_service;
  service.next_service += SERVICE_SIZE;

  pd->vmem_base = base;
  pd->vmem_size = SERVICE_SIZE;
  pd->entry_point += base;
  pd->stack_bottom += base;
  pd->stack_top += base;
  pd->local_bottom += base;
  for (unsigned i = 0; i < pd->n_maps; ++i) {
    struct process_memory *pm = pd->memory_maps + i;
    pm->v_base += base;
  }

  return base;
}

uint64_t service_register(uint64_t target) {
  SPIN_GUARD_RAW(service.lock);
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
