#ifndef _SYS_UIO_H
#define _SYS_UIO_H

// The Open Group Base Specifications Issue 7

/*
The <sys/uio.h> header shall define the iovec structure, which shall include at least the following members:

void   *iov_base  Base address of a memory region for input or output. 
size_t  iov_len   The size of the memory pointed to by iov_base. 

The <sys/uio.h> header uses the iovec structure for scatter/gather I/O.

The <sys/uio.h> header shall define the ssize_t and size_t types as described in <sys/types.h>.

The following shall be declared as functions and may also be defined as macros. Function prototypes shall be provided.

ssize_t readv(int, const struct iovec *, int);
ssize_t writev(int, const struct iovec *, int);
*/

#endif
