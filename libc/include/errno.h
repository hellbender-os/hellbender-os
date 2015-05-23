#ifndef _ERRNO_H
#define _ERRNO_H

// The Open Group Base Specifications Issue 7

// This depends on THREAD_LOCAL_PAGE in "kernel.h", and threadlocal.h struct.
#define errno (*((int*)0x402000))

/*
[E2BIG]
Argument list too long.
[EACCES]
Permission denied.
[EADDRINUSE]
Address in use.
[EADDRNOTAVAIL]
Address not available.
[EAFNOSUPPORT]
Address family not supported.
[EAGAIN]
Resource unavailable, try again (may be the same value as [EWOULDBLOCK]).
[EALREADY]
Connection already in progress.
[EBADF]
Bad file descriptor.
[EBADMSG]
Bad message.
[EBUSY]
Device or resource busy.
[ECANCELED]
Operation canceled.
[ECHILD]
No child processes.
[ECONNABORTED]
Connection aborted.
[ECONNREFUSED]
Connection refused.
[ECONNRESET]
Connection reset.
[EDEADLK]
Resource deadlock would occur.
[EDESTADDRREQ]
Destination address required.
[EDOM]
Mathematics argument out of domain of function.
[EDQUOT]
Reserved.
[EEXIST]
File exists.
[EFAULT]
Bad address.
[EFBIG]
File too large.
[EHOSTUNREACH]
Host is unreachable.
[EIDRM]
Identifier removed.
[EILSEQ]
Illegal byte sequence.
[EINPROGRESS]
Operation in progress.
[EINTR]
Interrupted function.
*/
#define EINVAL 12
// Invalid argument.
/*
[EIO]
I/O error.
[EISCONN]
Socket is connected.
[EISDIR]
Is a directory.
[ELOOP]
Too many levels of symbolic links.
[EMFILE]
File descriptor value too large.
[EMLINK]
Too many links.
[EMSGSIZE]
Message too large.
[EMULTIHOP]
Reserved.
[ENAMETOOLONG]
Filename too long.
[ENETDOWN]
Network is down.
[ENETRESET]
Connection aborted by network.
[ENETUNREACH]
Network unreachable.
[ENFILE]
Too many files open in system.
[ENOBUFS]
No buffer space available.
[ENODATA]
[OB XSR] [Option Start] No message is available on the STREAM head read queue. [Option End]
[ENODEV]
No such device.
[ENOENT]
No such file or directory.
[ENOEXEC]
Executable file format error.
[ENOLCK]
No locks available.
[ENOLINK]
Reserved.
[ENOMEM]
Not enough space.
[ENOMSG]
No message of the desired type.
[ENOPROTOOPT]
Protocol not available.
[ENOSPC]
No space left on device.
[ENOSR]
[OB XSR] [Option Start] No STREAM resources. [Option End]
[ENOSTR]
[OB XSR] [Option Start] Not a STREAM. [Option End]
[ENOSYS]
Function not supported.
[ENOTCONN]
The socket is not connected.
[ENOTDIR]
Not a directory or a symbolic link to a directory.
[ENOTEMPTY]
Directory not empty.
[ENOTRECOVERABLE]
State not recoverable.
[ENOTSOCK]
Not a socket.
[ENOTSUP]
Not supported (may be the same value as [EOPNOTSUPP]).
[ENOTTY]
Inappropriate I/O control operation.
[ENXIO]
No such device or address.
[EOPNOTSUPP]
Operation not supported on socket (may be the same value as [ENOTSUP]).
[EOVERFLOW]
Value too large to be stored in data type.
[EOWNERDEAD]
Previous owner died.
[EPERM]
Operation not permitted.
[EPIPE]
Broken pipe.
[EPROTO]
Protocol error.
[EPROTONOSUPPORT]
Protocol not supported.
[EPROTOTYPE]
Protocol wrong type for socket.
*/
#define ERANGE 123
// Result too large.
/*
[EROFS]
Read-only file system.
[ESPIPE]
Invalid seek.
[ESRCH]
No such process.
[ESTALE]
Reserved.
[ETIME]
[OB XSR] [Option Start] Stream ioctl() timeout. [Option End]
[ETIMEDOUT]
Connection timed out.
[ETXTBSY]
Text file busy.
[EWOULDBLOCK]
Operation would block (may be the same value as [EAGAIN]).
[EXDEV]
Cross-device link.
*/

#endif
