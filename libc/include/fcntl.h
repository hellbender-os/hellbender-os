#ifndef _FCNTL_H
#define _FCNTL_H

// SEEK_SET, SEEK_CUR, and SEEK_END as described in <stdio.h>.
// symbolic constants for file modes for use as values of mode_t as described in <sys/stat.h>.

// The Open Group Base Specifications Issue 7

#include <unistd.h>
#include <sys/stat.h>

#define F_DUPFD 1
// Duplicate file descriptor.

#define F_DUPFD_CLOEXEC 2
// Duplicate file descriptor with the close-on- exec flag FD_CLOEXEC set.

#define F_GETFD 3
// Get file descriptor flags.

#define F_SETFD 4
// Set file descriptor flags.

#define F_GETFL 5
// Get file status flags and file access modes.

#define F_SETFL 6
// Set file status flags.

#define F_GETLK 7
// Get record locking information.

#define F_SETLK 8
// Set record locking information.

#define F_SETLKW 9
// Set record locking information; wait if blocked.

#define F_GETOWN 10
// Get process or process group ID to receive SIGURG signals.

#define F_SETOWN 11
// Set process or process group ID to receive SIGURG signals.

#define FD_CLOEXEC 0x10000
// Close the file descriptor upon execution of an exec family function.

#define F_RDLCK 1
// Shared or read lock.

#define F_UNLCK 2
// Unlock.

#define F_WRLCK 3
// Exclusive or write lock.

#define O_CLOEXEC   0x10000
// The FD_CLOEXEC flag associated with the new descriptor shall be set to close the file descriptor upon execution of an exec family function.
// Must be FD_CLOEXEC.

#define O_CREAT     0x00001
// Create file if it does not exist.

#define O_DIRECTORY 0x00002
// Fail if not a directory.

#define O_EXCL      0x00004
// Exclusive use flag.

#define O_NOCTTY    0x00008
// Do not assign controlling terminal.

#define O_NOFOLLOW  0x00010
// Do not follow symbolic links.

#define O_TRUNC     0x00020
// Truncate flag.

#define O_TTY_INIT  0x00040
// Set the termios structure terminal parameters to a state that provides conforming behavior; see Parameters that Can be Set.
// The O_TTY_INIT flag can have the value zero and in this case it need not be bitwise-distinct from the other flags.

#define _O_STATFLAGS 0x00f80

#define O_APPEND    0x00080
// Set append mode.

#define O_DSYNC     0x00100
// [SIO] [Option Start] Write according to synchronized I/O data integrity completion. [Option End]

#define O_NONBLOCK  0x00200
// Non-blocking mode.

#define O_RSYNC     0x00400
// [SIO] [Option Start] Synchronized read I/O operations. [Option End]

#define O_SYNC      0x00800
// Write according to synchronized I/O file integrity completion.

#define O_ACCMODE   0x0f000
// Mask for file access modes.

#define O_EXEC      0x01000
// Open for execute only (non-directory files). The result is unspecified if this flag is applied to a directory.

#define O_RDONLY    0x02000
// Open for reading only.

#define O_RDWR      0x06000
// Open for reading and writing.

#define O_SEARCH    0x08000
// Open directory for search only. The result is unspecified if this flag is applied to a non-directory file.

#define O_WRONLY    0x04000
// Open for writing only.

#define AT_FDCWD OPEN_MAX
// Use the current working directory to determine the target of relative file paths.

#define AT_EACCESS 0x20000
// Check access using effective user and group ID.

#define AT_SYMLINK_NOFOLLOW 1
// Do not follow symbolic links.

#define AT_SYMLINK_FOLLOW 1
// Follow symbolic link.

#define AT_REMOVEDIR 1
// Remove directory instead of file.

#define POSIX_FADV_DONTNEED 1
// The application expects that it will not access the specified data in the near future.

#define POSIX_FADV_NOREUSE 2
// The application expects to access the specified data once and then not reuse it thereafter.

#define POSIX_FADV_NORMAL 3
// The application has no advice to give on its behavior with respect to the specified data. It is the default characteristic if no advice is given for an open file.

#define POSIX_FADV_RANDOM 4
// The application expects to access the specified data in a random order.

#define POSIX_FADV_SEQUENTIAL 5
// The application expects to access the specified data sequentially from lower offsets to higher offsets.

#define POSIX_FADV_WILLNEED 6
// The application expects to access the specified data in the near future.

struct flock {
  short  l_type; //   Type of lock; F_RDLCK, F_WRLCK, F_UNLCK. 
  short  l_whence; // Flag for starting offset. 
  off_t  l_start; //  Relative offset in bytes. 
  off_t  l_len; //    Size; if 0 then until EOF. 
  pid_t  l_pid; //    Process ID of the process holding the lock; returned with F_GETLK.
};

// mode_t, off_t, and pid_t types as described in <sys/types.h>.

int  creat(const char *, mode_t);
int  fcntl(int, int, ...);
int  open(const char *, int, ...);
int  openat(int, const char *, int, ...);
int  posix_fadvise(int, off_t, off_t, int);
int  posix_fallocate(int, off_t, off_t);

#endif
