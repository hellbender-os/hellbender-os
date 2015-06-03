#include <string.h>
#include <errno.h>

#define E(nr, str) case nr: return str;

char *strerror_l(int error, locale_t locale) {
  (void)(locale); //TODO: support
  return strerror(error);
}

int strerror_r(int error, char *buf, size_t size) {
  char* str = strerror(error);
  size_t len = strlen(str);
  if (size < len + 1) {
    return ERANGE;
  }
  memcpy(buf, str, len + 1);
  return 0;
}

char *strerror(int error) {
  switch (error) {
    E(E2BIG, "Argument list too long.");
    E(EACCES, "Permission denied.");
    E(EADDRINUSE, "Address in use.");
    E(EADDRNOTAVAIL, "Address not available.");
    E(EAFNOSUPPORT, "Address family not supported.");
    E(EAGAIN, "Resource unavailable, try again.");
    E(EALREADY, "Connection already in progress.");
    E(EBADF, "Bad file descriptor.");
    E(EBADMSG, "Bad message.");
    E(EBUSY, "Device or resource busy.");
    E(ECANCELED, "Operation canceled.");
    E(ECHILD, "No child processes.");
    E(ECONNABORTED, "Connection aborted.");
    E(ECONNREFUSED, "Connection refused.");
    E(ECONNRESET, "Connection reset.");
    E(EDEADLK, "Resource deadlock would occur.");
    E(EDESTADDRREQ, "Destination address required.");
    E(EDOM, "Mathematics argument out of domain of function.");
    E(EDQUOT, "Reserved.");
    E(EEXIST, "File exists.");
    E(EFAULT, "Bad address.");
    E(EFBIG, "File too large.");
    E(EHOSTUNREACH, "Host is unreachable.");
    E(EIDRM, "Identifier removed.");
    E(EILSEQ, "Illegal byte sequence.");
    E(EINPROGRESS, "Operation in progress.");
    E(EINTR, "Interrupted function.");
    E(EINVAL, "Invalid argument.");
    E(EIO, "I/O error.");
    E(EISCONN, "Socket is connected.");
    E(EISDIR, "Is a directory.");
    E(ELOOP, "Too many levels of symbolic links.");
    E(EMFILE, "File descriptor value too large.");
    E(EMLINK, "Too many links.");
    E(EMSGSIZE, "Message too large.");
    E(EMULTIHOP, "Reserved.");
    E(ENAMETOOLONG, "Filename too long.");
    E(ENETDOWN, "Network is down.");
    E(ENETRESET, "Connection aborted by network.");
    E(ENETUNREACH, "Network unreachable.");
    E(ENFILE, "Too many files open in system.");
    E(ENOBUFS, "No buffer space available.");
    E(ENODATA, "No message is available on the STREAM head read queue.");
    E(ENODEV, "No such device.");
    E(ENOENT, "No such file or directory.");
    E(ENOEXEC, "Executable file format error.");
    E(ENOLCK, "No locks available.");
    E(ENOLINK, "Reserved.");
    E(ENOMEM, "Not enough space.");
    E(ENOMSG, "No message of the desired type.");
    E(ENOPROTOOPT, "Protocol not available.");
    E(ENOSPC, "No space left on device.");
    E(ENOSR, "No STREAM resources.");
    E(ENOSTR, "Not a STREAM.");
    E(ENOSYS, "Function not supported.");
    E(ENOTCONN, "The socket is not connected.");
    E(ENOTDIR, "Not a directory or a symbolic link to a directory.");
    E(ENOTEMPTY, "Directory not empty.");
    E(ENOTRECOVERABLE, "State not recoverable.");
    E(ENOTSOCK, "Not a socket.");
    E(ENOTSUP, "Not supported.");
    E(ENOTTY, "Inappropriate I/O control operation.");
    E(ENXIO, "No such device or address.");
    E(EOPNOTSUPP, "Operation not supported on socket.");
    E(EOVERFLOW, "Value too large to be stored in data type.");
    E(EOWNERDEAD, "Previous owner died.");
    E(EPERM, "Operation not permitted.");
    E(EPIPE, "Broken pipe.");
    E(EPROTO, "Protocol error.");
    E(EPROTONOSUPPORT, "Protocol not supported.");
    E(EPROTOTYPE, "Protocol wrong type for socket.");
    E(ERANGE, "Result too large.");
    E(EROFS, "Read-only file system.");
    E(ESPIPE, "Invalid seek.");
    E(ESRCH, "No such process.");
    E(ESTALE, "Reserved.");
    E(ETIME, "Stream ioctl() timeout.");
    E(ETIMEDOUT, "Connection timed out.");
    E(ETXTBSY, "Text file busy.");
    E(EWOULDBLOCK, "Operation would block.");
    E(EXDEV, "Cross-device link.");
  default:
    errno = EINVAL;
    return "Unknown error code.";
  }
}
