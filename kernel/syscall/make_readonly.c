#include <kernel/syscall.h>
#include <kernel/mmap.h>
#include <kernel/domain.h>

void syscall_make_readonly(void* address, unsigned size) {
    // TODO: memory must by contained within thread address space.
    mmap_remap(address, size, MMAP_ATTRIB_USER_RO);
    domain_t *domain = domain_from_address(address);
    domain_update_data(domain, address, (size_t)size);
}

