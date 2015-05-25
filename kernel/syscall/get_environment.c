#include <kernel/syscall.h>
#include <kernel/thread.h>

void syscall_get_environment(int *argc, int *envc, char** strings) {
  domain_restore_environment(CURRENT_THREAD->home_domain,
                             argc, envc, strings);
}
