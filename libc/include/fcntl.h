#ifndef _FCNTL_H
#define _FCNTL_H

// The Open Group Base Specifications Issue 7

#include <unistd.h>
#include <sys/stat.h>

/*
F_DUPFD
Duplicate file descriptor.
F_DUPFD_CLOEXEC
Duplicate file descriptor with the close-on- exec flag FD_CLOEXEC set.
F_GETFD
Get file descriptor flags.
F_SETFD
Set file descriptor flags.
F_GETFL
Get file status flags and file access modes.
F_SETFL
Set file status flags.
F_GETLK
Get record locking information.
F_SETLK
Set record locking information.
F_SETLKW
Set record locking information; wait if blocked.
F_GETOWN
Get process or process group ID to receive SIGURG signals.
F_SETOWN
Set process or process group ID to receive SIGURG signals.

FD_CLOEXEC
Close the file descriptor upon execution of an exec family function.
The <fcntl.h> header shall also define the following symbolic constants for the l_type argument used for record locking with fcntl(). The values shall be unique and shall be suitable for use in #if preprocessing directives.

F_RDLCK
Shared or read lock.
F_UNLCK
Unlock.
F_WRLCK
Exclusive or write lock.
*/

// SEEK_SET, SEEK_CUR, and SEEK_END as described in <stdio.h>.

//O_CLOEXEC
//The FD_CLOEXEC flag associated with the new descriptor shall be set to close the file descriptor upon execution of an exec family function.

#define O_CREAT     0x0001
//Create file if it does not exist.

//O_DIRECTORY
//Fail if not a directory.

#define O_EXCL      0x0002
//Exclusive use flag.

#define O_NOCTTY    0x0004
//Do not assign controlling terminal.

//O_NOFOLLOW
//Do not follow symbolic links.

#define O_TRUNC     0x0010
//Truncate flag.

//O_TTY_INIT
//Set the termios structure terminal parameters to a state that provides conforming behavior; see Parameters that Can be Set.
//The O_TTY_INIT flag can have the value zero and in this case it need not be bitwise-distinct from the other flags.

#define O_APPEND    0x0020
//Set append mode.

//O_DSYNC
//[SIO] [Option Start] Write according to synchronized I/O data integrity completion. [Option End]

#define O_NONBLCK   0x0200
//Non-blocking mode.

#define O_RSYNC     0x0400
//[SIO] [Option Start] Synchronized read I/O operations. [Option End]

#define O_SYNC      0x0800
//Write according to synchronized I/O file integrity completion.

#define O_ACCMODE   0xf000
//Mask for file access modes.

//O_EXEC
//Open for execute only (non-directory files). The result is unspecified if this flag is applied to a directory.

#define O_RDONLY    0x1000
//Open for reading only.

#define O_RDWR      0x2000
//Open for reading and writing.

//O_SEARCH
//Open directory for search only. The result is unspecified if this flag is applied to a non-directory file.

#define O_WRONLY    0x4000
//Open for writing only.

// symbolic constants for file modes for use as values of mode_t as described in <sys/stat.h>.

/*
AT_FDCWD
Use the current working directory to determine the target of relative file paths.
The <fcntl.h> header shall define the following symbolic constant as a value for the flag used by faccessat():

AT_EACCESS
Check access using effective user and group ID.

AT_SYMLINK_NOFOLLOW
Do not follow symbolic links.

AT_SYMLINK_FOLLOW
Follow symbolic link.

AT_REMOVEDIR
Remove directory instead of file.
*/

/*
POSIX_FADV_DONTNEED
The application expects that it will not access the specified data in the near future.
POSIX_FADV_NOREUSE
The application expects to access the specified data once and then not reuse it thereafter.
POSIX_FADV_NORMAL
The application has no advice to give on its behavior with respect to the specified data. It is the default characteristic if no advice is given for an open file.
POSIX_FADV_RANDOM
The application expects to access the specified data in a random order.
POSIX_FADV_SEQUENTIAL
The application expects to access the specified data sequentially from lower offsets to higher offsets.
POSIX_FADV_WILLNEED
The application expects to access the specified data in the near future.
*/

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
