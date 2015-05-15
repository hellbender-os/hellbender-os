#ifndef _LIMITS_H
#define _LIMITS_H

/*
{AIO_LISTIO_MAX}
Maximum number of I/O operations in a single list I/O call supported by the implementation.
Minimum Acceptable Value: {_POSIX_AIO_LISTIO_MAX} [Option End]
{AIO_MAX}
Maximum number of outstanding asynchronous I/O operations supported by the implementation.
Minimum Acceptable Value: {_POSIX_AIO_MAX} [Option End]
{AIO_PRIO_DELTA_MAX}
The maximum amount by which a process can decrease its asynchronous I/O priority level from its own scheduling priority.
Minimum Acceptable Value: 0 [Option End]
{ARG_MAX}
Maximum length of argument to the exec functions including environment data.
Minimum Acceptable Value: {_POSIX_ARG_MAX}
{ATEXIT_MAX}
Maximum number of functions that may be registered with atexit().
Minimum Acceptable Value: 32 [Option End]
{CHILD_MAX}
Maximum number of simultaneous processes per real user ID.
Minimum Acceptable Value: {_POSIX_CHILD_MAX}
{DELAYTIMER_MAX}
Maximum number of timer expiration overruns.
Minimum Acceptable Value: {_POSIX_DELAYTIMER_MAX} [Option End]
{HOST_NAME_MAX}
Maximum length of a host name (not including the terminating null) as returned from the gethostname() function.
Minimum Acceptable Value: {_POSIX_HOST_NAME_MAX}
{IOV_MAX}
Maximum number of iovec structures that one process has available for use with readv() or writev().
Minimum Acceptable Value: {_XOPEN_IOV_MAX} [Option End]
{LOGIN_NAME_MAX}
Maximum length of a login name.
Minimum Acceptable Value: {_POSIX_LOGIN_NAME_MAX}
{MQ_OPEN_MAX}
The maximum number of open message queue descriptors a process may hold.
Minimum Acceptable Value: {_POSIX_MQ_OPEN_MAX} [Option End]
{MQ_PRIO_MAX}
The maximum number of message priorities supported by the implementation.
Minimum Acceptable Value: {_POSIX_MQ_PRIO_MAX} [Option End]
*/
#define OPEN_MAX 20
#define PAGESIZE 4096
#define PAGE_SIZE 4096
/*
{PTHREAD_DESTRUCTOR_ITERATIONS}
Maximum number of attempts made to destroy a thread's thread-specific data values on thread exit.
Minimum Acceptable Value: {_POSIX_THREAD_DESTRUCTOR_ITERATIONS} [Option End]
{PTHREAD_KEYS_MAX}
Maximum number of data keys that can be created by a process.
Minimum Acceptable Value: {_POSIX_THREAD_KEYS_MAX} [Option End]
{PTHREAD_STACK_MIN}
Minimum size in bytes of thread stack storage.
Minimum Acceptable Value: 0 [Option End]
{PTHREAD_THREADS_MAX}
Maximum number of threads that can be created per process.
Minimum Acceptable Value: {_POSIX_THREAD_THREADS_MAX} [Option End]
{RE_DUP_MAX}
The number of repeated occurrences of a BRE permitted by the regexec() and regcomp() functions when using the interval notation {\(m,n\}; see BREs Matching Multiple Characters.
Minimum Acceptable Value: {_POSIX2_RE_DUP_MAX}
{RTSIG_MAX}
Maximum number of realtime signals reserved for application use in this implementation.
Minimum Acceptable Value: {_POSIX_RTSIG_MAX} [Option End]
{SEM_NSEMS_MAX}
Maximum number of semaphores that a process may have.
Minimum Acceptable Value: {_POSIX_SEM_NSEMS_MAX} [Option End]
{SEM_VALUE_MAX}
The maximum value a semaphore may have.
Minimum Acceptable Value: {_POSIX_SEM_VALUE_MAX} [Option End]
{SIGQUEUE_MAX}
Maximum number of queued signals that a process may send and have pending at the receiver(s) at any time.
Minimum Acceptable Value: {_POSIX_SIGQUEUE_MAX} [Option End]
{SS_REPL_MAX}
The maximum number of replenishment operations that may be simultaneously pending for a particular sporadic server scheduler.
Minimum Acceptable Value: {_POSIX_SS_REPL_MAX} [Option End]
{STREAM_MAX}
The number of streams that one process can have open at one time. If defined, it has the same value as {FOPEN_MAX} (see <stdio.h>).
Minimum Acceptable Value: {_POSIX_STREAM_MAX}
{SYMLOOP_MAX}
Maximum number of symbolic links that can be reliably traversed in the resolution of a pathname in the absence of a loop.
Minimum Acceptable Value: {_POSIX_SYMLOOP_MAX}
{TIMER_MAX}
Maximum number of timers per process supported by the implementation.
Minimum Acceptable Value: {_POSIX_TIMER_MAX} [Option End]
{TRACE_EVENT_NAME_MAX}
Maximum length of the trace event name.
Minimum Acceptable Value: {_POSIX_TRACE_EVENT_NAME_MAX} [Option End]
{TRACE_NAME_MAX}
Maximum length of the trace generation version string or of the trace stream name.
Minimum Acceptable Value: {_POSIX_TRACE_NAME_MAX} [Option End]
{TRACE_SYS_MAX}
Maximum number of trace streams that may simultaneously exist in the system.
Minimum Acceptable Value: {_POSIX_TRACE_SYS_MAX} [Option End]
{TRACE_USER_EVENT_MAX}
Maximum number of user trace event type identifiers that may simultaneously exist in a traced process, including the predefined user trace event POSIX_TRACE_UNNAMED_USER_EVENT.
Minimum Acceptable Value: {_POSIX_TRACE_USER_EVENT_MAX} [Option End]
{TTY_NAME_MAX}
Maximum length of terminal device name.
Minimum Acceptable Value: {_POSIX_TTY_NAME_MAX}
*/
#define TZNAME_MAX 6
/*

{FILESIZEBITS}
Minimum number of bits needed to represent, as a signed integer value, the maximum size of a regular file allowed in the specified directory.
Minimum Acceptable Value: 32
{LINK_MAX}
Maximum number of links to a single file.
Minimum Acceptable Value: {_POSIX_LINK_MAX}
{MAX_CANON}
Maximum number of bytes in a terminal canonical input line.
Minimum Acceptable Value: {_POSIX_MAX_CANON}
{MAX_INPUT}
Minimum number of bytes for which space is available in a terminal input queue; therefore, the maximum number of bytes a conforming application may require to be typed as input before reading them.
Minimum Acceptable Value: {_POSIX_MAX_INPUT}
*/
#define NAME_MAX 14
#define PATH_MAX 256
/*
{PIPE_BUF}
Maximum number of bytes that is guaranteed to be atomic when writing to a pipe.
Minimum Acceptable Value: {_POSIX_PIPE_BUF}
{POSIX_ALLOC_SIZE_MIN}
[ADV] [Option Start]
Minimum number of bytes of storage actually allocated for any portion of a file.
Minimum Acceptable Value: Not specified. [Option End]
{POSIX_REC_INCR_XFER_SIZE}
[ADV] [Option Start]
Recommended increment for file transfer sizes between the {POSIX_REC_MIN_XFER_SIZE} and {POSIX_REC_MAX_XFER_SIZE} values.
Minimum Acceptable Value: Not specified. [Option End]
{POSIX_REC_MAX_XFER_SIZE}
[ADV] [Option Start]
Maximum recommended file transfer size.
Minimum Acceptable Value: Not specified. [Option End]
{POSIX_REC_MIN_XFER_SIZE}
[ADV] [Option Start]
Minimum recommended file transfer size.
Minimum Acceptable Value: Not specified. [Option End]
{POSIX_REC_XFER_ALIGN}
[ADV] [Option Start]
Recommended file transfer buffer alignment.
Minimum Acceptable Value: Not specified. [Option End]
{SYMLINK_MAX}
Maximum number of bytes in a symbolic link.
Minimum Acceptable Value: {_POSIX_SYMLINK_MAX}


{BC_BASE_MAX}
Maximum obase values allowed by the bc utility.
Minimum Acceptable Value: {_POSIX2_BC_BASE_MAX}
{BC_DIM_MAX}
Maximum number of elements permitted in an array by the bc utility.
Minimum Acceptable Value: {_POSIX2_BC_DIM_MAX}
{BC_SCALE_MAX}
Maximum scale value allowed by the bc utility.
Minimum Acceptable Value: {_POSIX2_BC_SCALE_MAX}
{BC_STRING_MAX}
Maximum length of a string constant accepted by the bc utility.
Minimum Acceptable Value: {_POSIX2_BC_STRING_MAX}
{CHARCLASS_NAME_MAX}
Maximum number of bytes in a character class name.
Minimum Acceptable Value: {_POSIX2_CHARCLASS_NAME_MAX}
{COLL_WEIGHTS_MAX}
Maximum number of weights that can be assigned to an entry of the LC_COLLATE order keyword in the locale definition file; see Locale.
Minimum Acceptable Value: {_POSIX2_COLL_WEIGHTS_MAX}
{EXPR_NEST_MAX}
Maximum number of expressions that can be nested within parentheses by the expr utility.
Minimum Acceptable Value: {_POSIX2_EXPR_NEST_MAX}
{LINE_MAX}
Unless otherwise noted, the maximum length, in bytes, of a utility's input line (either standard input or another file), when the utility is described as processing text files. The length includes room for the trailing <newline>.
Minimum Acceptable Value: {_POSIX2_LINE_MAX}
{NGROUPS_MAX}
Maximum number of simultaneous supplementary group IDs per process.
Minimum Acceptable Value: {_POSIX_NGROUPS_MAX}
{RE_DUP_MAX}
Maximum number of repeated occurrences of a regular expression permitted when using the interval notation \{m,n\}; see Regular Expressions.
Minimum Acceptable Value: {_POSIX2_RE_DUP_MAX}


{_POSIX_CLOCKRES_MIN}


{_POSIX_AIO_LISTIO_MAX}
The number of I/O operations that can be specified in a list I/O call.
Value: 2 [Option End]
{_POSIX_AIO_MAX}
The number of outstanding asynchronous I/O operations.
Value: 1 [Option End]
{_POSIX_ARG_MAX}
Maximum length of argument to the exec functions including environment data.
Value: 4 096
{_POSIX_CHILD_MAX}
Maximum number of simultaneous processes per real user ID.
Value: 25
{_POSIX_DELAYTIMER_MAX}
The number of timer expiration overruns.
Value: 32 [Option End]
{_POSIX_HOST_NAME_MAX}
Maximum length of a host name (not including the terminating null) as returned from the gethostname() function.
Value: 255
{_POSIX_LINK_MAX}
Maximum number of links to a single file.
Value: 8
{_POSIX_LOGIN_NAME_MAX}
The size of the storage required for a login name, in bytes, including the terminating null.
Value: 9
{_POSIX_MAX_CANON}
Maximum number of bytes in a terminal canonical input queue.
Value: 255
{_POSIX_MAX_INPUT}
Maximum number of bytes allowed in a terminal input queue.
Value: 255
{_POSIX_MQ_OPEN_MAX}
The number of message queues that can be open for a single process.
Value: 8 [Option End]
{_POSIX_MQ_PRIO_MAX}
The maximum number of message priorities supported by the implementation.
Value: 32 [Option End]
*/
#define _POSIX_NAME_MAX 14
/*
{_POSIX_NGROUPS_MAX}
Maximum number of simultaneous supplementary group IDs per process.
Value: 8
*/
#define _POSIX_OPEN_MAX 20
#define _POSIX_PATH_MAX 256
/*
{_POSIX_PIPE_BUF}
Maximum number of bytes that is guaranteed to be atomic when writing to a pipe.
Value: 512
{_POSIX_RE_DUP_MAX}
The number of repeated occurrences of a BRE permitted by the regexec() and regcomp() functions when using the interval notation {\(m,n\}; see BREs Matching Multiple Characters.
Value: 255
{_POSIX_RTSIG_MAX}
The number of realtime signal numbers reserved for application use.
Value: 8 [Option End]
{_POSIX_SEM_NSEMS_MAX}
The number of semaphores that a process may have.
Value: 256 [Option End]
{_POSIX_SEM_VALUE_MAX}
The maximum value a semaphore may have.
Value: 32 767 [Option End]
{_POSIX_SIGQUEUE_MAX}
The number of queued signals that a process may send and have pending at the receiver(s) at any time.
Value: 32 [Option End]
{_POSIX_SSIZE_MAX}
The value that can be stored in an object of type ssize_t.
Value: 32 767
{_POSIX_STREAM_MAX}
The number of streams that one process can have open at one time.
Value: 8
{_POSIX_SS_REPL_MAX}
The number of replenishment operations that may be simultaneously pending for a particular sporadic server scheduler.
Value: 4 [Option End]
{_POSIX_SYMLINK_MAX}
The number of bytes in a symbolic link.
Value: 255
{_POSIX_SYMLOOP_MAX}
The number of symbolic links that can be traversed in the resolution of a pathname in the absence of a loop.
Value: 8
{_POSIX_THREAD_DESTRUCTOR_ITERATIONS}
The number of attempts made to destroy a thread's thread-specific data values on thread exit.
Value: 4 [Option End]
{_POSIX_THREAD_KEYS_MAX}
The number of data keys per process.
Value: 128 [Option End]
{_POSIX_THREAD_THREADS_MAX}
The number of threads per process.
Value: 64 [Option End]
{_POSIX_TIMER_MAX}
The per-process number of timers.
Value: 32 [Option End]
{_POSIX_TRACE_EVENT_NAME_MAX}
The length in bytes of a trace event name.
Value: 30 [Option End]
{_POSIX_TRACE_NAME_MAX}
The length in bytes of a trace generation version string or a trace stream name.
Value: 8 [Option End]
{_POSIX_TRACE_SYS_MAX}
The number of trace streams that may simultaneously exist in the system.
Value: 8 [Option End]
{_POSIX_TRACE_USER_EVENT_MAX}
The number of user trace event type identifiers that may simultaneously exist in a traced process, including the predefined user trace event POSIX_TRACE_UNNAMED_USER_EVENT.
Value: 32 [Option End]
{_POSIX_TTY_NAME_MAX}
The size of the storage required for a terminal device name, in bytes, including the terminating null.
Value: 9
*/
#define _POSIX_TZNAME_MAX 6
/*
{_POSIX2_BC_BASE_MAX}
Maximum obase values allowed by the bc utility.
Value: 99
{_POSIX2_BC_DIM_MAX}
Maximum number of elements permitted in an array by the bc utility.
Value: 2 048
{_POSIX2_BC_SCALE_MAX}
Maximum scale value allowed by the bc utility.
Value: 99
{_POSIX2_BC_STRING_MAX}
Maximum length of a string constant accepted by the bc utility.
Value: 1 000
{_POSIX2_CHARCLASS_NAME_MAX}
Maximum number of bytes in a character class name.
Value: 14
{_POSIX2_COLL_WEIGHTS_MAX}
Maximum number of weights that can be assigned to an entry of the LC_COLLATE order keyword in the locale definition file; see Locale.
Value: 2
{_POSIX2_EXPR_NEST_MAX}
Maximum number of expressions that can be nested within parentheses by the expr utility.
Value: 32
{_POSIX2_LINE_MAX}
Unless otherwise noted, the maximum length, in bytes, of a utility's input line (either standard input or another file), when the utility is described as processing text files. The length includes room for the trailing <newline>.
Value: 2 048
{_POSIX2_RE_DUP_MAX]
Maximum number of repeated occurrences of a regular expression permitted when using the interval notation \{m,n\}; see Regular Expressions.
Value: 255
{_XOPEN_IOV_MAX}
Maximum number of iovec structures that one process has available for use with readv() or writev().
Value: 16 [Option End]
{_XOPEN_NAME_MAX}
Maximum number of bytes in a filename (not including the terminating null).
Value: 255 [Option End]
{_XOPEN_PATH_MAX}
Maximum number of bytes in a pathname.
Value: 1024 [Option End]

*/
#define CHAR_BIT 8
#define CHAR_MAX 127
#define CHAR_MIN -128
#define INT_MAX 2147483647
#define LONG_BIT 32
#define LONG_MAX 2147483647
/*
{MB_LEN_MAX}
Maximum number of bytes in a character, for any supported locale.
Minimum Acceptable Value: 1
*/
#define SCHAR_MAX 127
#define SHRT_MAX 32767
#define SSIZE_MAX 4294967295
#define UCHAR_MAX 255
#define UINT_MAX 4294967295
#define ULONG_MAX 4294967295
#define USHRT_MAX 65535
#define WORD_BIT 32
#define INT_MIN -2147483647
#define LONG_MIN -2147483647
#define SCHAR_MIN -128
#define SHRT_MIN -32767
#define LLONG_MIN -9223372036854775807
#define LLONG_MAX 9223372036854775807
#define ULLONG_MAX 18446744073709551615

/*
  {NL_ARGMAX}
Maximum value of digit in calls to the printf() and scanf() functions.
Minimum Acceptable Value: 9 [Option End]
{NL_LANGMAX}
Maximum number of bytes in a LANG name.
Minimum Acceptable Value: 14 [Option End]
{NL_MSGMAX}
Maximum message number.
Minimum Acceptable Value: 32 767 [Option End]
{NL_NMAX}
Maximum number of bytes in an N-to-1 collation mapping.
Minimum Acceptable Value: No guaranteed value across all conforming implementations. [Option End]
{NL_SETMAX}
Maximum set number.
Minimum Acceptable Value: 255 [Option End]
{NL_TEXTMAX}
Maximum number of bytes in a message string.
Minimum Acceptable Value: {_POSIX2_LINE_MAX} [Option End]
{NZERO}
Default process priority.
Minimum Acceptable Value: 20 [Option End]
*/
  
#endif
