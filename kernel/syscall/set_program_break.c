#include <kernel/syscall.h>
#include <kernel/thread.h>
#include <kernel/domain.h>

void* syscall_set_program_break(void* set_or_null, intptr_t delta_or_zero) {
  // TODO: memory must by contained within thread address space.
  domain_t *domain;
  if (set_or_null) {
    domain = domain_from_address(set_or_null);
  } else domain = CURRENT_THREAD->home_domain;
  void* retval = domain_set_break(domain, set_or_null, delta_or_zero);
  return retval;
}
