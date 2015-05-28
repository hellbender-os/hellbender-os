#ifndef _SYS_MMAN_H
#define _SYS_MMAN_H

#include <sys/types.h> // nonconforming: mode_t, off_t, and size_t

// The Open Group Base Specifications Issue 7

#define PROT_EXEC 1
// Page can be executed.

#define PROT_NONE 2
// Page cannot be accessed.

#define PROT_READ 3
// Page can be read.

#define PROT_WRITE 4
// Page can be written.

#define MAP_FIXED 1
// Interpret addr exactly.

#define MAP_PRIVATE 2
// Changes are private.

#define MAP_SHARED 3
// Share changes.

#define MS_ASYNC 1
// Perform asynchronous writes.

#define MS_INVALIDATE 2
// Invalidate mappings.

#define MS_SYNC 3
// Perform synchronous writes.

#define MCL_CURRENT 1
// Lock currently mapped pages.

#define MCL_FUTURE 2
// Lock pages that become mapped.

#define MAP_FAILED ((void*)0)

#define POSIX_MADV_DONTNEED 1
// The application expects that it will not access the specified range in the near future.

#define POSIX_MADV_NORMAL 2
// The application has no advice to give on its behavior with respect to the specified range. It is the default characteristic if no advice is given for a range of memory.

#define POSIX_MADV_RANDOM 3
// The application expects to access the specified range in a random order.

#define POSIX_MADV_SEQUENTIAL 4
// The application expects to access the specified range sequentially from lower addresses to higher addresses.

#define POSIX_MADV_WILLNEED 5
// The application expects to access the specified range in the near future.

#define POSIX_TYPED_MEM_ALLOCATE1 
// Allocate on mmap().

#define POSIX_TYPED_MEM_ALLOCATE_CONTIG 2
// Allocate contiguously on mmap().

#define POSIX_TYPED_MEM_MAP_ALLOCATABLE 3
// Map on mmap(), without affecting allocatability.

struct posix_typed_mem_info {
  size_t  posix_tmi_length; //  Maximum length which may be allocated from a typed memory int.
};

int    mlock(const void *, size_t);
int    mlockall(int);
void  *mmap(void *, size_t, int, int, int, off_t);
int    mprotect(void *, size_t, int);
int    msync(void *, size_t, int);
int    munlock(const void *, size_t);
int    munlockall(void);
int    munmap(void *, size_t);
int    posix_madvise(void *, size_t, int);
int    posix_mem_offset(const void *, size_t, off_t *,
                        size_t *, int *);
int    posix_typed_mem_get_info(int, struct posix_typed_mem_info *);
int    posix_typed_mem_open(const char *, int, int);
int    shm_open(const char *, int, mode_t);
int    shm_unlink(const char *);

#endif
