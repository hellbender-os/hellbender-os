#ifndef _NETDB_H
#define _NETDB_H

#include <netinet/in.h>
#include <sys/socket.h>
#include <inttypes.h>
  
//The Open Group Base Specifications Issue 7

struct hostent {
  char   *h_name; //       Official name of the host. 
char  **h_aliases; //    A pointer to an array of pointers to 
                   //    alternative host names, terminated by a 
                   //    null pointer. 
int     h_addrtype; //   Address type. 
int     h_length; //     The length, in bytes, of the address. 
char  **h_addr_list; //  A pointer to an array of pointers to network 
                     //  addresses (in network byte order) for the host, 
                     //  terminated by a null pointer. 
};

struct netent {
  char     *n_name; //  Official, fully-qualified (including the 
                    //  domain) name of the host. 
  char    **n_aliases; //   A pointer to an array of pointers to 
                       //  alternative network names, terminated by a 
                       // null pointer. 
  int       n_addrtype; //  The address type of the network. 
  uint32_t  n_net; //       The network number, in host byte order. 
};

struct protoent {
  char   *p_name; //     Official name of the protocol. 
  char  **p_aliases; //  A pointer to an array of pointers to 
                     //  alternative protocol names, terminated by 
                     //  a null pointer. 
  int     p_proto; //    The protocol number. 
};

struct servent {
  char   *s_name; //     Official name of the service. 
  char  **s_aliases; //  A pointer to an array of pointers to 
                     //  alternative service names, terminated by 
                     //  a null pointer. 
  int     s_port; //     A value which, when converted to uint16_t, 
                  //     yields the port number in network byte order 
                  //     at which the service resides. 
  char   *s_proto; //    The name of the protocol to use when 
                   //    contacting the service. 
};

#define IPPORT_RESERVED 1023

struct addrinfo {
  int               ai_flags; //      Input flags. 
  int               ai_family; //     Address family of socket. 
  int               ai_socktype; //   Socket type. 
  int               ai_protocol; //   Protocol of socket. 
  socklen_t         ai_addrlen; //    Length of socket address. 
  struct sockaddr  *ai_addr; //       Socket address of socket. 
  char             *ai_canonname; //  Canonical name of service location. 
  struct addrinfo  *ai_next; //       Pointer to next in list. 
};

#define AI_PASSIVE 0x0001
//Socket address is intended for bind().

#define AI_CANONNAME 0x0002
//Request for canonical name.

#define AI_NUMERICHOST 0x0004
// Return numeric host address as name.

#define AI_NUMERICSERV 0x0008
// Inhibit service name resolution.

#define AI_V4MAPPED 0x0010
// If no IPv6 addresses are found, query for IPv4 addresses and return them to the caller as IPv4-mapped IPv6 addresses.

#define AI_ALL 0x0020
// Query for both IPv4 and IPv6 addresses.

#define AI_ADDRCONFIG 0x0040
// Query for IPv4 addresses only when an IPv4 address is configured; query for IPv6 addresses only when an IPv6 address is configured.

#define NI_NOFQDN 0x0001
// Only the nodename portion of the FQDN is returned for local hosts.

#define NI_NUMERICHOST 0x0002
// The numeric form of the node's address is returned instead of its name.

#define NI_NAMEREQD 0x0004
// Return an error if the node's name cannot be located in the database.

#define NI_NUMERICSERV 0x0008
// The numeric form of the service address is returned instead of its name.

#define NI_NUMERICSCOPE 0x0010
// For IPv6 addresses, the numeric form of the scope identifier is returned instead of its name.

#define NI_DGRAM 0x0020
// Indicates that the service is a datagram service (SOCK_DGRAM).

#define EAI_AGAIN 100
// The name could not be resolved at this time. Future attempts may succeed.

#define EAI_BADFLAGS 101
// The flags had an invalid value.

#define EAI_FAIL 102
// A non-recoverable error occurred.

#define EAI_FAMILY 103
// The address family was not recognized or the address length was invalid for the specified family.

#define EAI_MEMORY 104
// There was a memory allocation failure.

#define EAI_NONAME 105
// The name does not resolve for the supplied parameters.
// NI_NAMEREQD is set and the host's name cannot be located, or both nodename and servname were null.

#define EAI_SERVICE 106
// The service passed was not recognized for the specified socket type.

#define EAI_SOCKTYPE 107
// The intended socket type was not recognized.

#define EAI_SYSTEM 108
// A system error occurred. The error code can be found in errno.

#define EAI_OVERFLOW 109
// An argument buffer overflowed.

void              endhostent(void);
void              endnetent(void);
void              endprotoent(void);
void              endservent(void);
void              freeaddrinfo(struct addrinfo *);
const char       *gai_strerror(int);
int               getaddrinfo(const char *, const char *,
                              const struct addrinfo *,
                              struct addrinfo **);
struct hostent   *gethostent(void);
int               getnameinfo(const struct sockaddr *, socklen_t,
                              char *, socklen_t, char *,
                              socklen_t, int);
struct netent    *getnetbyaddr(uint32_t, int);
struct netent    *getnetbyname(const char *);
struct netent    *getnetent(void);
struct protoent  *getprotobyname(const char *);
struct protoent  *getprotobynumber(int);
struct protoent  *getprotoent(void);
struct servent   *getservbyname(const char *, const char *);
struct servent   *getservbyport(int, const char *);
struct servent   *getservent(void);
void              sethostent(int);
void              setnetent(int);
void              setprotoent(int);
void              setservent(int);

#endif
