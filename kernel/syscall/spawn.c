#include <kernel/syscall.h>
#include <kernel/process.h>
#include <kernel/domain.h>
#include <kernel/thread.h>

int syscall_spawn(const char *path, char *const argv[], char *const envp[]) {
  process_t *process = process_create_application(path);
  domain_store_environment(process->domain, argv, envp);
  return process->thread->thread_id;
}
