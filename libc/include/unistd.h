#ifndef _UNISTD_H
#define _UNISTD_H

#include <sys/types.h> // nonconforming: size_t, ssize_t, uid_t, gid_t, off_t, and pid_t
#include <stddef.h> // nonconforming: NULL
#include <stdio.h> // nonconforming:  SEEK_CUR, SEEK_END, and SEEK_SET
#include <inttypes.h> // nonconforming intptr_t

// The Open Group Base Specifications Issue 7

//_POSIX_VERSION
// Integer value indicating version of this standard (C-language binding) to which the implementation conforms. For implementations conforming to POSIX.1-2008, the value shall be 200809L.

//_POSIX2_VERSION
// Integer value indicating version of the Shell and Utilities volume of POSIX.1 to which the implementation conforms. For implementations conforming to POSIX.1-2008, the value shall be 200809L. For profile implementations that define _POSIX_SUBPROFILE (see Subprofiling Considerations) in <unistd.h>, POSIX2_VERSION may be left undefined or be defined with the value -1 to indicate that the Shell and Utilities volume of POSIX.1 is not supported. In this case, a call to sysconf(_SC_2_VERSION) shall return either 200809L or -1 indicating that the Shell and Utilities volume of POSIX.1 is or is not, respectively, supported at runtime.

//_XOPEN_VERSION
// Integer value indicating version of the X/Open Portability Guide to which the implementation conforms. The value shall be 700. [Option End]

//_POSIX_ADVISORY_INFO
// The implementation supports the Advisory Information option. If this symbol is defined in <unistd.h>, it shall be defined to be -1, 0, or 200809L. The value of this symbol reported by sysconf() shall either be -1 or 200809L. [Option End]

//_POSIX_ASYNCHRONOUS_IO
// The implementation supports asynchronous input and output. This symbol shall always be set to the value 200809L.

//_POSIX_BARRIERS
// The implementation supports barriers. This symbol shall always be set to the value 200809L.

//_POSIX_CHOWN_RESTRICTED
// The use of chown() and fchown() is restricted to a process with appropriate privileges, and to changing the group ID of a file only to the effective group ID of the process or to one of its supplementary group IDs. This symbol shall be defined with a value other than -1.

//_POSIX_CLOCK_SELECTION
// The implementation supports clock selection. This symbol shall always be set to the value 200809L.

//_POSIX_CPUTIME
// The implementation supports the Process CPU-Time Clocks option. If this symbol is defined in <unistd.h>, it shall be defined to be -1, 0, or 200809L. The value of this symbol reported by sysconf() shall either be -1 or 200809L. [Option End]

//_POSIX_FSYNC
// The implementation supports the File Synchronization option. If this symbol is defined in <unistd.h>, it shall be defined to be -1, 0, or 200809L. The value of this symbol reported by sysconf() shall either be -1 or 200809L. [Option End]

//_POSIX_IPV6
// The implementation supports the IPv6 option. If this symbol is defined in <unistd.h>, it shall be defined to be -1, 0, or 200809L. The value of this symbol reported by sysconf() shall either be -1 or 200809L.

//_POSIX_JOB_CONTROL
// The implementation supports job control. This symbol shall always be set to a value greater than zero.

//_POSIX_MAPPED_FILES
// The implementation supports memory mapped Files. This symbol shall always be set to the value 200809L.

//_POSIX_MEMLOCK
// The implementation supports the Process Memory Locking option. If this symbol is defined in <unistd.h>, it shall be defined to be -1, 0, or 200809L. The value of this symbol reported by sysconf() shall either be -1 or 200809L. [Option End]

//_POSIX_MEMLOCK_RANGE
// The implementation supports the Range Memory Locking option. If this symbol is defined in <unistd.h>, it shall be defined to be -1, 0, or 200809L. The value of this symbol reported by sysconf() shall either be -1 or 200809L. [Option End]

//_POSIX_MEMORY_PROTECTION
// The implementation supports memory protection. This symbol shall always be set to the value 200809L.

//_POSIX_MESSAGE_PASSING
// The implementation supports the Message Passing option. If this symbol is defined in <unistd.h>, it shall be defined to be -1, 0, or 200809L. The value of this symbol reported by sysconf() shall either be -1 or 200809L. [Option End]

//_POSIX_MONOTONIC_CLOCK
// The implementation supports the Monotonic Clock option. If this symbol is defined in <unistd.h>, it shall be defined to be -1, 0, or 200809L. The value of this symbol reported by sysconf() shall either be -1 or 200809L. [Option End]

//_POSIX_NO_TRUNC
// Pathname components longer than {NAME_MAX} generate an error. This symbol shall be defined with a value other than -1.

//_POSIX_PRIORITIZED_IO
// The implementation supports the Prioritized Input and Output option. If this symbol is defined in <unistd.h>, it shall be defined to be -1, 0, or 200809L. The value of this symbol reported by sysconf() shall either be -1 or 200809L. [Option End]

//_POSIX_PRIORITY_SCHEDULING
// The implementation supports the Process Scheduling option. If this symbol is defined in <unistd.h>, it shall be defined to be -1, 0, or 200809L. The value of this symbol reported by sysconf() shall either be -1 or 200809L. [Option End]

//_POSIX_RAW_SOCKETS
// The implementation supports the Raw Sockets option. If this symbol is defined in <unistd.h>, it shall be defined to be -1, 0, or 200809L. The value of this symbol reported by sysconf() shall either be -1 or 200809L. [Option End]

//_POSIX_READER_WRITER_LOCKS
// The implementation supports read-write locks. This symbol shall always be set to the value 200809L.

//_POSIX_REALTIME_SIGNALS
// The implementation supports realtime signals. This symbol shall always be set to the value 200809L.

//_POSIX_REGEXP
// The implementation supports the Regular Expression Handling option. This symbol shall always be set to a value greater than zero.

//_POSIX_SAVED_IDS
// Each process has a saved set-user-ID and a saved set-group-ID. This symbol shall always be set to a value greater than zero.

//_POSIX_SEMAPHORES
// The implementation supports semaphores. This symbol shall always be set to the value 200809L.

//_POSIX_SHARED_MEMORY_OBJECTS
// The implementation supports the Shared Memory Objects option. If this symbol is defined in <unistd.h>, it shall be defined to be -1, 0, or 200809L. The value of this symbol reported by sysconf() shall either be -1 or 200809L. [Option End]

//_POSIX_SHELL
// The implementation supports the POSIX shell. This symbol shall always be set to a value greater than zero.

//_POSIX_SPAWN
// The implementation supports the Spawn option. If this symbol is defined in <unistd.h>, it shall be defined to be -1, 0, or 200809L. The value of this symbol reported by sysconf() shall either be -1 or 200809L. [Option End]

//_POSIX_SPIN_LOCKS
// The implementation supports spin locks. This symbol shall always be set to the value 200809L.

//_POSIX_SPORADIC_SERVER
// The implementation supports the Process Sporadic Server option. If this symbol is defined in <unistd.h>, it shall be defined to be -1, 0, or 200809L. The value of this symbol reported by sysconf() shall either be -1 or 200809L. [Option End]

//_POSIX_SYNCHRONIZED_IO
// The implementation supports the Synchronized Input and Output option. If this symbol is defined in <unistd.h>, it shall be defined to be -1, 0, or 200809L. The value of this symbol reported by sysconf() shall either be -1 or 200809L. [Option End]

//_POSIX_THREAD_ATTR_STACKADDR
// The implementation supports the Thread Stack Address Attribute option. If this symbol is defined in <unistd.h>, it shall be defined to be -1, 0, or 200809L. The value of this symbol reported by sysconf() shall either be -1 or 200809L. [Option End]

//_POSIX_THREAD_ATTR_STACKSIZE
// The implementation supports the Thread Stack Size Attribute option. If this symbol is defined in <unistd.h>, it shall be defined to be -1, 0, or 200809L. The value of this symbol reported by sysconf() shall either be -1 or 200809L. [Option End]

//_POSIX_THREAD_CPUTIME
// The implementation supports the Thread CPU-Time Clocks option. If this symbol is defined in <unistd.h>, it shall be defined to be -1, 0, or 200809L. The value of this symbol reported by sysconf() shall either be -1 or 200809L. [Option End]

//_POSIX_THREAD_PRIO_INHERIT
// The implementation supports the Non-Robust Mutex Priority Inheritance option. If this symbol is defined in <unistd.h>, it shall be defined to be -1, 0, or 200809L. The value of this symbol reported by sysconf() shall either be -1 or 200809L. [Option End]

//_POSIX_THREAD_PRIO_PROTECT
//The implementation supports the Non-Robust Mutex Priority Protection option. If this symbol is defined in <unistd.h>, it shall be defined to be -1, 0, or 200809L. The value of this symbol reported by sysconf() shall either be -1 or 200809L. [Option End]

//_POSIX_THREAD_PRIORITY_SCHEDULING
// The implementation supports the Thread Execution Scheduling option. If this symbol is defined in <unistd.h>, it shall be defined to be -1, 0, or 200809L. The value of this symbol reported by sysconf() shall either be -1 or 200809L. [Option End]

//_POSIX_THREAD_PROCESS_SHARED
// The implementation supports the Thread Process-Shared Synchronization option. If this symbol is defined in <unistd.h>, it shall be defined to be -1, 0, or 200809L. The value of this symbol reported by sysconf() shall either be -1 or 200809L. [Option End]

//_POSIX_THREAD_ROBUST_PRIO_INHERIT
// The implementation supports the Robust Mutex Priority Inheritance option. If this symbol is defined in <unistd.h>, it shall be defined to be -1, 0, or 200809L. The value of this symbol reported by sysconf() shall either be -1 or 200809L. [Option End]

//_POSIX_THREAD_ROBUST_PRIO_PROTECT
// The implementation supports the Robust Mutex Priority Protection option. If this symbol is defined in <unistd.h>, it shall be defined to be -1, 0, or 200809L. The value of this symbol reported by sysconf() shall either be -1 or 200809L. [Option End]

//_POSIX_THREAD_SAFE_FUNCTIONS
// The implementation supports thread-safe functions. This symbol shall always be set to the value 200809L.

//_POSIX_THREAD_SPORADIC_SERVER
// The implementation supports the Thread Sporadic Server option. If this symbol is defined in <unistd.h>, it shall be defined to be -1, 0, or 200809L. The value of this symbol reported by sysconf() shall either be -1 or 200809L. [Option End]

//_POSIX_THREADS
// The implementation supports threads. This symbol shall always be set to the value 200809L.

//_POSIX_TIMEOUTS
// The implementation supports timeouts. This symbol shall always be set to the value 200809L.

//_POSIX_TIMERS
// The implementation supports timers. This symbol shall always be set to the value 200809L.

//_POSIX_TRACE
// The implementation supports the Trace option. If this symbol is defined in <unistd.h>, it shall be defined to be -1, 0, or 200809L. The value of this symbol reported by sysconf() shall either be -1 or 200809L. [Option End]

//_POSIX_TRACE_EVENT_FILTER
// The implementation supports the Trace Event Filter option. If this symbol is defined in <unistd.h>, it shall be defined to be -1, 0, or 200809L. The value of this symbol reported by sysconf() shall either be -1 or 200809L. [Option End]

//_POSIX_TRACE_INHERIT
// The implementation supports the Trace Inherit option. If this symbol is defined in <unistd.h>, it shall be defined to be -1, 0, or 200809L. The value of this symbol reported by sysconf() shall either be -1 or 200809L. [Option End]

//_POSIX_TRACE_LOG
// The implementation supports the Trace Log option. If this symbol is defined in <unistd.h>, it shall be defined to be -1, 0, or 200809L. The value of this symbol reported by sysconf() shall either be -1 or 200809L. [Option End]

//_POSIX_TYPED_MEMORY_OBJECTS
// The implementation supports the Typed Memory Objects option. If this symbol is defined in <unistd.h>, it shall be defined to be -1, 0, or 200809L. The value of this symbol reported by sysconf() shall either be -1 or 200809L. [Option End]

//_POSIX_V6_ILP32_OFF32
// The implementation provides a C-language compilation environment with 32-bit int, long, pointer, and off_t types. [Option End]

//_POSIX_V6_ILP32_OFFBIG
// The implementation provides a C-language compilation environment with 32-bit int, long, and pointer types and an off_t type using at least 64 bits. [Option End]

//_POSIX_V6_LP64_OFF64
// The implementation provides a C-language compilation environment with 32-bit int and 64-bit long, pointer, and off_t types. [Option End]

//_POSIX_V6_LPBIG_OFFBIG
// The implementation provides a C-language compilation environment with an int type using at least 32 bits and long, pointer, and off_t types using at least 64 bits. [Option End]

//_POSIX_V7_ILP32_OFF32
// The implementation provides a C-language compilation environment with 32-bit int, long, pointer, and off_t types.

//_POSIX_V7_ILP32_OFFBIG
// The implementation provides a C-language compilation environment with 32-bit int, long, and pointer types and an off_t type using at least 64 bits.

//_POSIX_V7_LP64_OFF64
//The implementation provides a C-language compilation environment with 32-bit int and 64-bit long, pointer, and off_t types.

//_POSIX_V7_LPBIG_OFFBIG
// The implementation provides a C-language compilation environment with an int type using at least 32 bits and long, pointer, and off_t types using at least 64 bits.

//_POSIX2_C_BIND
// The implementation supports the C-Language Binding option. This symbol shall always have the value 200809L.

//_POSIX2_C_DEV
//  The implementation supports the C-Language Development Utilities option. If this symbol is defined in <unistd.h>, it shall be defined to be -1, 0, or 200809L. The value of this symbol reported by sysconf() shall either be -1 or 200809L. [Option End]

//_POSIX2_CHAR_TERM
// The implementation supports the Terminal Characteristics option. The value of this symbol reported by sysconf() shall either be -1 or a value greater than zero.

//_POSIX2_FORT_DEV
// The implementation supports the FORTRAN Development Utilities option. If this symbol is defined in <unistd.h>, it shall be defined to be -1, 0, or 200809L. The value of this symbol reported by sysconf() shall either be -1 or 200809L. [Option End]

//_POSIX2_FORT_RUN
// The implementation supports the FORTRAN Runtime Utilities option. If this symbol is defined in <unistd.h>, it shall be defined to be -1, 0, or 200809L. The value of this symbol reported by sysconf() shall either be -1 or 200809L. [Option End]

//_POSIX2_LOCALEDEF
// The implementation supports the creation of locales by the localedef utility. If this symbol is defined in <unistd.h>, it shall be defined to be -1, 0, or 200809L. The value of this symbol reported by sysconf() shall either be -1 or 200809L.

//_POSIX2_PBS
// The implementation supports the Batch Environment Services and Utilities option. If this symbol is defined in <unistd.h>, it shall be defined to be -1, 0, or 200809L. The value of this symbol reported by sysconf() shall either be -1 or 200809L. [Option End]

//_POSIX2_PBS_ACCOUNTING
// The implementation supports the Batch Accounting option. If this symbol is defined in <unistd.h>, it shall be defined to be -1, 0, or 200809L. The value of this symbol reported by sysconf() shall either be -1 or 200809L. [Option End]

//_POSIX2_PBS_CHECKPOINT
// The implementation supports the Batch Checkpoint/Restart option. If this symbol is defined in <unistd.h>, it shall be defined to be -1, 0, or 200809L. The value of this symbol reported by sysconf() shall either be -1 or 200809L. [Option End]

//_POSIX2_PBS_LOCATE
// The implementation supports the Locate Batch Job Request option. If this symbol is defined in <unistd.h>, it shall be defined to be -1, 0, or 200809L. The value of this symbol reported by sysconf() shall either be -1 or 200809L. [Option End]

//_POSIX2_PBS_MESSAGE
// The implementation supports the Batch Job Message Request option. If this symbol is defined in <unistd.h>, it shall be defined to be -1, 0, or 200809L. The value of this symbol reported by sysconf() shall either be -1 or 200809L. [Option End]

//_POSIX2_PBS_TRACK
// The implementation supports the Track Batch Job Request option. If this symbol is defined in <unistd.h>, it shall be defined to be -1, 0, or 200809L. The value of this symbol reported by sysconf() shall either be -1 or 200809L. [Option End]

//_POSIX2_SW_DEV
// The implementation supports the Software Development Utilities option. If this symbol is defined in <unistd.h>, it shall be defined to be -1, 0, or 200809L. The value of this symbol reported by sysconf() shall either be -1 or 200809L. [Option End]

//_POSIX2_UPE
// The implementation supports the User Portability Utilities option. If this symbol is defined in <unistd.h>, it shall be defined to be -1, 0, or 200809L. The value of this symbol reported by sysconf() shall either be -1 or 200809L. [Option End]

//_XOPEN_CRYPT
// The implementation supports the X/Open Encryption Option Group.

//_XOPEN_ENH_I18N
// The implementation supports the Issue 4, Version 2 Enhanced Internationalization Option Group. This symbol shall always be set to a value other than -1.

//_XOPEN_REALTIME
// The implementation supports the X/Open Realtime Option Group.

//_XOPEN_REALTIME_THREADS
// The implementation supports the X/Open Realtime Threads Option Group.

//_XOPEN_SHM
// The implementation supports the Issue 4, Version 2 Shared Memory Option Group. This symbol shall always be set to a value other than -1. [Option End]

//_XOPEN_STREAMS
// The implementation supports the XSI STREAMS Option Group. [Option End]

//_XOPEN_UNIX
// The implementation supports the XSI option. [Option End]

//_XOPEN_UUCP
// The implementation supports the UUCP Utilities option. If this symbol is defined in <unistd.h>, it shall be defined to be -1, 0, or 200809L. The value of this symbol reported by sysconf() shall be either -1 or 200809L. [Option End]

//_POSIX_ASYNC_IO
// Asynchronous input or output operations may be performed for the associated file.

//_POSIX_PRIO_IO
// Prioritized input or output operations may be performed for the associated file.

//_POSIX_SYNC_IO
// Synchronized input or output operations may be performed for the associated file.

//_POSIX_TIMESTAMP_RESOLUTION
// The resolution in nanoseconds for all file timestamps.

//_POSIX2_SYMLINKS
// Symbolic links can be created.

#define F_OK 0x1
// Test for existence of file.

#define R_OK 0x2
// Test for read permission.

#define W_OK 0x4
// Test for write permission.

#define X_OK 0x8
// Test for execute (search) permission.

//_CS_PATH
// This is the value for the PATH environment variable that finds all standard utilities.

//_CS_POSIX_V7_ILP32_OFF32_CFLAGS
// If sysconf(_SC_V7_ILP32_OFF32) returns -1, the meaning of this value is unspecified. Otherwise, this value is the set of initial options to be given to the c99 utility to build an application using a programming model with 32-bit int, long, pointer, and off_t types.

//_CS_POSIX_V7_ILP32_OFF32_LDFLAGS
// If sysconf(_SC_V7_ILP32_OFF32) returns -1, the meaning of this value is unspecified. Otherwise, this value is the set of final options to be given to the c99 utility to build an application using a programming model with 32-bit int, long, pointer, and off_t types.

//_CS_POSIX_V7_ILP32_OFF32_LIBS
// If sysconf(_SC_V7_ILP32_OFF32) returns -1, the meaning of this value is unspecified. Otherwise, this value is the set of libraries to be given to the c99 utility to build an application using a programming model with 32-bit int, long, pointer, and off_t types.

//_CS_POSIX_V7_ILP32_OFFBIG_CFLAGS
// If sysconf(_SC_V7_ILP32_OFFBIG) returns -1, the meaning of this value is unspecified. Otherwise, this value is the set of initial options to be given to the c99 utility to build an application using a programming model with 32-bit int, long, and pointer types, and an off_t type using at least 64 bits.

//_CS_POSIX_V7_ILP32_OFFBIG_LDFLAGS
// If sysconf(_SC_V7_ILP32_OFFBIG) returns -1, the meaning of this value is unspecified. Otherwise, this value is the set of final options to be given to the c99 utility to build an application using a programming model with 32-bit int, long, and pointer types, and an off_t type using at least 64 bits.

//_CS_POSIX_V7_ILP32_OFFBIG_LIBS
// If sysconf(_SC_V7_ILP32_OFFBIG) returns -1, the meaning of this value is unspecified. Otherwise, this value is the set of libraries to be given to the c99 utility to build an application using a programming model with 32-bit int, long, and pointer types, and an off_t type using at least 64 bits.

//_CS_POSIX_V7_LP64_OFF64_CFLAGS
// If sysconf(_SC_V7_LP64_OFF64) returns -1, the meaning of this value is unspecified. Otherwise, this value is the set of initial options to be given to the c99 utility to build an application using a programming model with 32-bit int and 64-bit long, pointer, and off_t types.

//_CS_POSIX_V7_LP64_OFF64_LDFLAGS
// If sysconf(_SC_V7_LP64_OFF64) returns -1, the meaning of this value is unspecified. Otherwise, this value is the set of final options to be given to the c99 utility to build an application using a programming model with 32-bit int and 64-bit long, pointer, and off_t types.

//_CS_POSIX_V7_LP64_OFF64_LIBS
// If sysconf(_SC_V7_LP64_OFF64) returns -1, the meaning of this value is unspecified. Otherwise, this value is the set of libraries to be given to the c99 utility to build an application using a programming model with 32-bit int and 64-bit long, pointer, and off_t types.

//_CS_POSIX_V7_LPBIG_OFFBIG_CFLAGS
// If sysconf(_SC_V7_LPBIG_OFFBIG) returns -1, the meaning of this value is unspecified. Otherwise, this value is the set of initial options to be given to the c99 utility to build an application using a programming model with an int type using at least 32 bits and long, pointer, and off_t types using at least 64 bits.

//_CS_POSIX_V7_LPBIG_OFFBIG_LDFLAGS
// If sysconf(_SC_V7_LPBIG_OFFBIG) returns -1, the meaning of this value is unspecified. Otherwise, this value is the set of final options to be given to the c99 utility to build an application using a programming model with an int type using at least 32 bits and long, pointer, and off_t types using at least 64 bits.

//_CS_POSIX_V7_LPBIG_OFFBIG_LIBS
// If sysconf(_SC_V7_LPBIG_OFFBIG) returns -1, the meaning of this value is unspecified. Otherwise, this value is the set of libraries to be given to the c99 utility to build an application using a programming model with an int type using at least 32 bits and long, pointer, and off_t types using at least 64 bits.

//_CS_POSIX_V7_THREADS_CFLAGS
// If sysconf(_SC_POSIX_THREADS) returns -1, the meaning of this value is unspecified. Otherwise, this value is the set of initial options to be given to the c99 utility to build a multi-threaded application. These flags are in addition to those associated with any of the other _CS_POSIX_V7_*_CFLAGS values used to specify particular type size programing environments.

//_CS_POSIX_V7_THREADS_LDFLAGS
// If sysconf(_SC_POSIX_THREADS) returns -1, the meaning of this value is unspecified. Otherwise, this value is the set of final options to be given to the c99 utility to build a multi-threaded application. These flags are in addition to those associated with any of the other _CS_POSIX_V7_*_LDFLAGS values used to specify particular type size programing environments.

//_CS_POSIX_V7_WIDTH_RESTRICTED_ENVS
// This value is a <newline>-separated list of names of programming environments supported by the implementation in which the widths of the blksize_t, cc_t, mode_t, nfds_t, pid_t, ptrdiff_t, size_t, speed_t, ssize_t, suseconds_t, tcflag_t, wchar_t, and wint_t types are no greater than the width of type long. The format of each name shall be suitable for use with the getconf -v option.

//_CS_V7_ENV
// This is the value that provides the environment variable information (other than that provided by _CS_PATH) that is required by the implementation to create a conforming environment, as described in the implementation's conformance documentation.

/*
_CS_POSIX_V6_ILP32_OFF32_CFLAGS
_CS_POSIX_V6_ILP32_OFF32_LDFLAGS
_CS_POSIX_V6_ILP32_OFF32_LIBS
_CS_POSIX_V6_ILP32_OFFBIG_CFLAGS
_CS_POSIX_V6_ILP32_OFFBIG_LDFLAGS
_CS_POSIX_V6_ILP32_OFFBIG_LIBS
_CS_POSIX_V6_LP64_OFF64_CFLAGS
_CS_POSIX_V6_LP64_OFF64_LDFLAGS
_CS_POSIX_V6_LP64_OFF64_LIBS
_CS_POSIX_V6_LPBIG_OFFBIG_CFLAGS
_CS_POSIX_V6_LPBIG_OFFBIG_LDFLAGS
_CS_POSIX_V6_LPBIG_OFFBIG_LIBS
_CS_POSIX_V6_WIDTH_RESTRICTED_ENVS
_CS_V6_ENV
*/

#define F_LOCK 1
//Lock a section for exclusive use.

#define F_TEST 2
// Test section for locks by other processes.

#define F_TLOCK 3
// Test and lock a section for exclusive use.

#define F_ULOCK 4
// Unlock locked sections.

#define _PC_2_SYMLINKS 1
#define _PC_ALLOC_SIZE_MIN 2
#define _PC_ASYNC_IO 3
#define _PC_CHOWN_RESTRICTED 4
#define _PC_FILESIZEBITS 5
#define _PC_LINK_MAX 6
#define _PC_MAX_CANON 7
#define _PC_MAX_INPUT 8
#define _PC_NAME_MAX 9
#define _PC_NO_TRUNC 10
#define _PC_PATH_MAX 11
#define _PC_PIPE_BUF 12
#define _PC_PRIO_IO 13
#define _PC_REC_INCR_XFER_SIZE 14
#define _PC_REC_MAX_XFER_SIZE 15
#define _PC_REC_MIN_XFER_SIZE 16
#define _PC_REC_XFER_ALIGN 17
#define _PC_SYMLINK_MAX 18
#define _PC_SYNC_IO 19
#define _PC_TIMESTAMP_RESOLUTION 20
#define _PC_VDISABLE 21

#define _SC_2_C_BIND 22
#define _SC_2_C_DEV 23
#define _SC_2_CHAR_TERM 24
#define _SC_2_FORT_DEV 25
#define _SC_2_FORT_RUN 26
#define _SC_2_LOCALEDEF 27
#define _SC_2_PBS 28
#define _SC_2_PBS_ACCOUNTING 29
#define _SC_2_PBS_CHECKPOINT 30
#define _SC_2_PBS_LOCATE 31
#define _SC_2_PBS_MESSAGE 32
#define _SC_2_PBS_TRACK 33
#define _SC_2_SW_DEV 34
#define _SC_2_UPE 35
#define _SC_2_VERSION 36
#define _SC_ADVISORY_INFO 37
#define _SC_AIO_LISTIO_MAX 38
#define _SC_AIO_MAX 39
#define _SC_AIO_PRIO_DELTA_MAX 40
#define _SC_ARG_MAX 41
#define _SC_ASYNCHRONOUS_IO 42
#define _SC_ATEXIT_MAX 43
#define _SC_BARRIERS 44
#define _SC_BC_BASE_MAX 45
#define _SC_BC_DIM_MAX 46
#define _SC_BC_SCALE_MAX 47
#define _SC_BC_STRING_MAX 48
#define _SC_CHILD_MAX 49
#define _SC_CLK_TCK 50
#define _SC_CLOCK_SELECTION 51
#define _SC_COLL_WEIGHTS_MAX 52
#define _SC_CPUTIME 53
#define _SC_DELAYTIMER_MAX 54
#define _SC_EXPR_NEST_MAX 55
#define _SC_FSYNC 56
#define _SC_GETGR_R_SIZE_MAX 57
#define _SC_GETPW_R_SIZE_MAX 58
#define _SC_HOST_NAME_MAX 59
#define _SC_IOV_MAX 60
#define _SC_IPV6 61
#define _SC_JOB_CONTROL 62
#define _SC_LINE_MAX 63
#define _SC_LOGIN_NAME_MAX 64
#define _SC_MAPPED_FILES 65
#define _SC_MEMLOCK 66
#define _SC_MEMLOCK_RANGE 67
#define _SC_MEMORY_PROTECTION 68
#define _SC_MESSAGE_PASSING 69
#define _SC_MONOTONIC_CLOCK 70
#define _SC_MQ_OPEN_MAX 71
#define _SC_MQ_PRIO_MAX 72
#define _SC_NGROUPS_MAX 73
#define _SC_OPEN_MAX 74
#define _SC_PAGE_SIZE 75
#define _SC_PAGESIZE 76
#define _SC_PRIORITIZED_IO 77
#define _SC_PRIORITY_SCHEDULING 78
#define _SC_RAW_SOCKETS 79
#define _SC_RE_DUP_MAX 80
#define _SC_READER_WRITER_LOCKS 81
#define _SC_REALTIME_SIGNALS 82
#define _SC_REGEXP 83
#define _SC_RTSIG_MAX 84
#define _SC_SAVED_IDS 85
#define _SC_SEM_NSEMS_MAX 86
#define _SC_SEM_VALUE_MAX 87
#define _SC_SEMAPHORES 88
#define _SC_SHARED_MEMORY_OBJECTS 89
#define _SC_SHELL 90
#define _SC_SIGQUEUE_MAX 91
#define _SC_SPAWN 92
#define _SC_SPIN_LOCKS 93
#define _SC_SPORADIC_SERVER 94
#define _SC_SS_REPL_MAX 95
#define _SC_STREAM_MAX 96
#define _SC_SYMLOOP_MAX 97
#define _SC_SYNCHRONIZED_IO 98
#define _SC_THREAD_ATTR_STACKADDR 99
#define _SC_THREAD_ATTR_STACKSIZE 100
#define _SC_THREAD_CPUTIME 101
#define _SC_THREAD_DESTRUCTOR_ITERATIONS 102
#define _SC_THREAD_KEYS_MAX 103
#define _SC_THREAD_PRIO_INHERIT 104
#define _SC_THREAD_PRIO_PROTECT 105
#define _SC_THREAD_PRIORITY_SCHEDULING 106
#define _SC_THREAD_PROCESS_SHARED 107
#define _SC_THREAD_ROBUST_PRIO_INHERIT 108
#define _SC_THREAD_ROBUST_PRIO_PROTECT 109
#define _SC_THREAD_SAFE_FUNCTIONS 110
#define _SC_THREAD_SPORADIC_SERVER 111
#define _SC_THREAD_STACK_MIN 112
#define _SC_THREAD_THREADS_MAX 113
#define _SC_THREADS 114
#define _SC_TIMEOUTS 115
#define _SC_TIMER_MAX 116
#define _SC_TIMERS 117
#define _SC_TRACE 118
#define _SC_TRACE_EVENT_FILTER 119
#define _SC_TRACE_EVENT_NAME_MAX 120
#define _SC_TRACE_INHERIT 121
#define _SC_TRACE_LOG 122
#define _SC_TRACE_NAME_MAX 123
#define _SC_TRACE_SYS_MAX 124
#define _SC_TRACE_USER_EVENT_MAX 125
#define _SC_TTY_NAME_MAX 126
#define _SC_TYPED_MEMORY_OBJECTS 127
#define _SC_TZNAME_MAX 128
#define _SC_V7_ILP32_OFF32 129
#define _SC_V7_ILP32_OFFBIG 130
#define _SC_V7_LP64_OFF64 131
#define _SC_V7_LPBIG_OFFBIG 132
#define _SC_V6_ILP32_OFF32 133
#define _SC_V6_ILP32_OFFBIG 134
#define _SC_V6_LP64_OFF64 135
#define _SC_V6_LPBIG_OFFBIG 136
#define _SC_VERSION 137
#define _SC_XOPEN_CRYPT 138
#define _SC_XOPEN_ENH_I18N 139
#define _SC_XOPEN_REALTIME 140
#define _SC_XOPEN_REALTIME_THREADS 141
#define _SC_XOPEN_SHM 142
#define _SC_XOPEN_STREAMS 143
#define _SC_XOPEN_UNIX 144
#define _SC_XOPEN_UUCP 145
#define _SC_XOPEN_VERSION 146

// _SC_PAGESIZE and _SC_PAGE_SIZE may be defined to have the same value.

#define STDERR_FILENO 2
//File number of stderr; 2.

#define STDIN_FILENO  0
//File number of stdin; 0.

#define STDOUT_FILENO 1
//File number of stdout; 1.

//_POSIX_VDISABLE
// This symbol shall be defined to be the value of a character that shall disable terminal special character handling as described in Special Control Characters. This symbol shall always be set to a value other than -1.

int          access(const char *, int);
unsigned     alarm(unsigned);
int          chdir(const char *);
int          chown(const char *, uid_t, gid_t);
int          close(int);
size_t       confstr(int, char *, size_t);
char        *crypt(const char *, const char *);
int          dup(int);
int          dup2(int, int);
void         _exit(int);
void         encrypt(char [64], int);
int          execl(const char *, const char *, ...);
int          execle(const char *, const char *, ...);
int          execlp(const char *, const char *, ...);
int          execv(const char *, char *const []);
int          execve(const char *, char *const [], char *const []);
int          execvp(const char *, char *const []);
int          faccessat(int, const char *, int, int);
int          fchdir(int);
int          fchown(int, uid_t, gid_t);
int          fchownat(int, const char *, uid_t, gid_t, int);
int          fdatasync(int);
int          fexecve(int, char *const [], char *const []);
pid_t        fork(void);
long         fpathconf(int, int);
int          fsync(int);
int          ftruncate(int, off_t);
char        *getcwd(char *, size_t);
gid_t        getegid(void);
uid_t        geteuid(void);
gid_t        getgid(void);
int          getgroups(int, gid_t []);
long         gethostid(void);
int          gethostname(char *, size_t);
char        *getlogin(void);
int          getlogin_r(char *, size_t);
int          getopt(int, char * const [], const char *);
pid_t        getpgid(pid_t);
pid_t        getpgrp(void);
pid_t        getpid(void);
pid_t        getppid(void);
pid_t        getsid(pid_t);
uid_t        getuid(void);
int          isatty(int);
int          lchown(const char *, uid_t, gid_t);
int          link(const char *, const char *);
int          linkat(int, const char *, int, const char *, int);
int          lockf(int, int, off_t);
off_t        lseek(int, off_t, int);
int          nice(int);
long         pathconf(const char *, int);
int          pause(void);
int          pipe(int [2]);
ssize_t      pread(int, void *, size_t, off_t);
ssize_t      pwrite(int, const void *, size_t, off_t);
ssize_t      read(int, void *, size_t);
ssize_t      readlink(const char *, char *, size_t);
ssize_t      readlinkat(int, const char *, char *, size_t);
int          rmdir(const char *);
int          setegid(gid_t);
int          seteuid(uid_t);
int          setgid(gid_t);
int          setpgid(pid_t, pid_t);
pid_t        setpgrp(void);
int          setregid(gid_t, gid_t);
int          setreuid(uid_t, uid_t);
pid_t        setsid(void);
int          setuid(uid_t);
unsigned     sleep(unsigned);
void         swab(const void *, void *, ssize_t);
int          symlink(const char *, const char *);
int          symlinkat(const char *, int, const char *);
void         sync(void);
long         sysconf(int);
pid_t        tcgetpgrp(int);
int          tcsetpgrp(int, pid_t);
int          truncate(const char *, off_t);
char        *ttyname(int);
int          ttyname_r(int, char *, size_t);
int          unlink(const char *);
int          unlinkat(int, const char *, int);
ssize_t      write(int, const void *, size_t);

//[OB] [Option Start] Implementations may also include the pthread_atfork() prototype as defined in <pthread.h>. Implementations may also include the ctermid() prototype as defined in <stdio.h>. [Option End]

extern char  *optarg;
extern int    opterr, optind, optopt;

#endif
