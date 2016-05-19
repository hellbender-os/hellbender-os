#ifndef _SYS_SOCKET_H
#define _SYS_SOCKET_H

#include <sys/types.h> // nonconforming: size_t, ssize_t
#define SOCK_RDM 100 // extra sock type.

// The Open Group Base Specifications Issue 7

#include <sys/uio.h>

typedef int socklen_t;
typedef unsigned sa_family_t;

struct sockaddr {
  sa_family_t  sa_family; //  Address family. 
  char         sa_data[]; //  Socket address (variable-length data). 
};

struct sockaddr_storage {
  sa_family_t   ss_family;
  char filler[32];
} __attribute__((aligned));
/*
Large enough to accommodate all supported protocol-specific address structures.
Aligned at an appropriate boundary so that pointers to it can be cast as pointers to protocol-specific address structures and used to access the fields of those structures without alignment problems
*/

struct msghdr {
  void          *msg_name; //        Optional address. 
  socklen_t      msg_namelen; //     Size of address. 
  struct iovec  *msg_iov; //         Scatter/gather array. 
  int            msg_iovlen; //      Members in msg_iov. 
  void          *msg_control; //     Ancillary data; see below. 
  socklen_t      msg_controllen; //  Ancillary data buffer len. 
  int            msg_flags; //       Flags on received message. 
};

struct cmsghdr {
  socklen_t  cmsg_len; //    Data byte count, including the cmsghdr. 
  int        cmsg_level; //  Originating protocol. 
  int        cmsg_type; //   Protocol-specific type. 
};
/*
The values for cmsg_level shall be legal values for the level argument to the getsockopt() and setsockopt() functions. The system documentation shall specify the cmsg_type definitions for the supported protocols.
*/

#define SCM_RIGHTS 1
//Indicates that the data array contains the access rights to be sent or received.

#define CMSG_DATA(cmsg) ((unsigned char*)(cmsg+1))
//If the argument is a pointer to a cmsghdr structure, this macro shall return an unsigned character pointer to the data array associated with the cmsghdr structure.

//#define CMSG_NXTHDR(mhdr,cmsg)
//If the first argument is a pointer to a msghdr structure and the second argument is a pointer to a cmsghdr structure in the ancillary data pointed to by the msg_control field of that msghdr structure, this macro shall return a pointer to the next cmsghdr structure, or a null pointer if this structure is the last cmsghdr in the ancillary data.

//#define CMSG_FIRSTHDR(mhdr)
//If the argument is a pointer to a msghdr structure, this macro shall return a pointer to the first cmsghdr structure in the ancillary data associated with this msghdr structure, or a null pointer if there is no ancillary data associated with the msghdr structure.

struct linger {
  int  l_onoff; //   Indicates whether linger option is enabled. 
  int  l_linger; //  Linger time, in seconds. 
};

#define SOCK_DGRAM 1
//Datagram socket.

#define SOCK_RAW 2
//[RS] [Option Start] Raw Protocol Interface. [Option End]

#define SOCK_SEQPACKET 3
// Sequenced-packet socket.

#define SOCK_STREAM 4
//Byte-stream socket.

#define SOL_SOCKET 1
//Options to be accessed at socket level, not protocol level.

#define SO_ACCEPTCONN 1
// Socket is accepting connections.

#define SO_BROADCAST 2
// Transmission of broadcast messages is supported.

#define SO_DEBUG 3
// Debugging information is being recorded.

#define SO_DONTROUTE 4
// Bypass normal routing.

#define SO_ERROR 5
// Socket error status.

#define SO_KEEPALIVE 6
// Connections are kept alive with periodic messages.

#define SO_LINGER 7
// Socket lingers on close.

#define SO_OOBINLINE 8
// Out-of-band data is transmitted in line.

#define SO_RCVBUF 9
// Receive buffer size.

#define SO_RCVLOWAT 10
// Receive ``low water mark''.

#define SO_RCVTIMEO 11
// Receive timeout.

#define SO_REUSEADDR 12
// Reuse of local addresses is supported.

#define SO_SNDBUF 13
// Send buffer size.

#define SO_SNDLOWAT 14
// Send ``low water mark''.

#define SO_SNDTIMEO 15
// Send timeout.

#define SO_TYPE 16
// Socket type.

#define SOMAXCONN 0
// The maximum backlog queue length.

#define MSG_CTRUNC 1
// Control data truncated.

#define MSG_DONTROUTE 2
// Send without using routing tables.

#define MSG_EOR 3
// Terminates a record (if supported by the protocol).

#define MSG_OOB 4
// Out-of-band data.

#define MSG_NOSIGNAL 5
// No SIGPIPE generated when an attempt to send is made on a stream-oriented socket that is no longer connected.

#define MSG_PEEK 6
// Leave received data in queue.

#define MSG_TRUNC 7
// Normal data truncated.

#define MSG_WAITALL 8
// Attempt to fill the read buffer.

#define AF_INET 1
// Internet domain sockets for use with IPv4 addresses.

#define AF_INET6 2
// [IP6] [Option Start] Internet domain sockets for use with IPv6 addresses. [Option End]

#define AF_UNIX 3
// UNIX domain sockets.

#define AF_UNSPEC 4
// Unspecified.

#define SHUT_RD 1
// Disables further receive operations.

#define SHUT_RDWR 2
// Disables further send and receive operations.

#define SHUT_WR 3
// Disables further send operations.

int     accept(int, struct sockaddr *, socklen_t *);
int     bind(int, const struct sockaddr *, socklen_t);
int     connect(int, const struct sockaddr *, socklen_t);
int     getpeername(int, struct sockaddr *, socklen_t *);
int     getsockname(int, struct sockaddr *, socklen_t *);
int     getsockopt(int, int, int, void *, socklen_t *);
int     listen(int, int);
ssize_t recv(int, void *, size_t, int);
ssize_t recvfrom(int, void *, size_t, int,
        struct sockaddr *, socklen_t *);
ssize_t recvmsg(int, struct msghdr *, int);
ssize_t send(int, const void *, size_t, int);
ssize_t sendmsg(int, const struct msghdr *, int);
ssize_t sendto(int, const void *, size_t, int, const struct sockaddr *,
        socklen_t);
int     setsockopt(int, int, int, const void *, socklen_t);
int     shutdown(int, int);
int     sockatmark(int);
int     socket(int, int, int);
int     socketpair(int, int, int, int [2]);

#endif
