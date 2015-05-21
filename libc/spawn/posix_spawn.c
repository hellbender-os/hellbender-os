#include <spawn.h>

#include <hellbender.h>

int posix_spawn(pid_t *pid, const char *path,
                const posix_spawn_file_actions_t *file_actions,
                const posix_spawnattr_t *attrp,
                char *const argv[], char *const envp[]) {
  //TODO: support actions and attributes.
  (void)(file_actions);
  (void)(attrp);
  
  // create new application process:
  //   create an application domain.
  //   mmap the boot strap code.
  //   create a thread for the domain.
  //   allocate physical page for args.
  //   copy args, envp into allocated memory.
  //   allocate kernel memory for path.
  //   copy path.
  //   store the path, args addresses in thread structure.

  *pid = syscall_spawn(path, argv, envp);
  
  return 0;
}

