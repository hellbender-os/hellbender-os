#include <kernel/syscall.h>
#include <kernel/mmap.h>
#include <kernel/domain.h>

void syscall_make_executable(void* address, unsigned size) {
    // TODO: memory must by contained within thread address space.
    // TODO: use malware scanning service.
  mmap_remap(address, size, MMAP_ATTRIB_USER_RO);
#if CS_BASE > 0
  mmap_mirror(address + CS_BASE, address, size);
#endif
  domain_t *domain = domain_from_address(address);
  domain_update_text(domain, address, (size_t)size);
}

