#ifndef _NETINET_IN_H
#define _NETINET_IN_H

// The Open Group Base Specifications Issue 7

#include <inttypes.h>

// resolving cross includes between in.h and inet.h
typedef uint16_t in_port_t;
typedef uint32_t in_addr_t;

#include <sys/socket.h>
#include <arpa/inet.h>

struct in_addr {
  in_addr_t  s_addr;
};

struct sockaddr_in {
  sa_family_t     sin_family; //   AF_INET. 
  in_port_t       sin_port; //     Port number. 
  struct in_addr  sin_addr; //     IP address. 
};
// The sin_port and sin_addr members shall be in network byte order.

struct in6_addr {
  uint8_t s6_addr[16];
};
// This array is used to contain a 128-bit IPv6 address, stored in network byte order.

struct sockaddr_in6 {
  sa_family_t      sin6_family; //    AF_INET6. 
  in_port_t        sin6_port; //      Port number. 
  uint32_t         sin6_flowinfo; //  IPv6 traffic class and flow information. 
  struct in6_addr  sin6_addr; //      IPv6 address. 
  uint32_t         sin6_scope_id; //  Set of interfaces for a scope. 
};
// The sin6_port and sin6_addr members shall be in network byte order.
// Implementations are free to have additional, implementation-defined fields in sockaddr_in6.
// The sin6_scope_id field is a 32-bit integer that identifies a set of interfaces as appropriate for the scope of the address carried in the sin6_addr field. For a link scope sin6_addr, the application shall ensure that sin6_scope_id is a link index. For a site scope sin6_addr, the application shall ensure that sin6_scope_id is a site index. The mapping of sin6_scope_id to an interface or set of interfaces is implementation-defined.

extern const struct in6_addr in6addr_any;
//This variable is initialized by the system to contain the wildcard IPv6 address.

//#define IN6ADDR_ANY_INIT
// This macro must be constant at compile time and can be used to initialize a variable of type struct in6_addr to the IPv6 wildcard address.

extern const struct in6_addr in6addr_loopback;
// This variable is initialized by the system to contain the loopback IPv6 address.

//#define IN6ADDR_LOOPBACK_INIT
// This macro must be constant at compile time and can be used to initialize a variable of type struct in6_addr to the IPv6 loopback address.

struct ipv6_mreq {
  struct in6_addr  ipv6mr_multiaddr; //  IPv6 multicast address. 
  unsigned         ipv6mr_interface; //  Interface index.
};

#define IPPROTO_IP 1
// Internet protocol.

#define IPPROTO_IPV6 2
// [IP6] [Option Start] Internet Protocol Version 6. [Option End]

#define IPPROTO_ICMP 3
// Control message protocol.

#define IPPROTO_RAW 4
// [RS] [Option Start] Raw IP Packets Protocol. [Option End]

#define IPPROTO_TCP 5
// Transmission control protocol.

#define IPPROTO_UDP 6
// User datagram protocol.

#define INADDR_ANY 0
// IPv4 local host address.
#define INADDR_BROADCAST 1
// IPv4 broadcast address.

#define INET_ADDRSTRLEN 16
// Length of the string form for IP.

#define INET6_ADDRSTRLEN 46
// Length of the string form for IPv6.

#define IPV6_JOIN_GROUP 1
// Join a multicast group.

#define IPV6_LEAVE_GROUP 2
// Quit a multicast group.

#define IPV6_MULTICAST_HOPS 3
// Multicast hop limit.

#define IPV6_MULTICAST_IF 4
// Interface to use for outgoing multicast packets.

#define IPV6_MULTICAST_LOOP 5
// Multicast packets are delivered back to the local application.

#define IPV6_UNICAST_HOPS 6
// Unicast hop limit.

#define IPV6_V6ONLY 7
// Restrict AF_INET6 socket to IPv6 communications only.

/*The <netinet/in.h> header shall define the following macros that test for special IPv6 addresses. Each macro is of type int and takes a single argument of type const struct in6_addr *:

IN6_IS_ADDR_UNSPECIFIED
Unspecified address.
IN6_IS_ADDR_LOOPBACK
Loopback address.
IN6_IS_ADDR_MULTICAST
Multicast address.
IN6_IS_ADDR_LINKLOCAL
Unicast link-local address.
IN6_IS_ADDR_SITELOCAL
Unicast site-local address.
IN6_IS_ADDR_V4MAPPED
IPv4 mapped address.
IN6_IS_ADDR_V4COMPAT
IPv4-compatible address.
IN6_IS_ADDR_MC_NODELOCAL
Multicast node-local address.
IN6_IS_ADDR_MC_LINKLOCAL
Multicast link-local address.
IN6_IS_ADDR_MC_SITELOCAL
Multicast site-local address.
IN6_IS_ADDR_MC_ORGLOCAL
Multicast organization-local address.
IN6_IS_ADDR_MC_GLOBAL
Multicast global address.
[Option End]
*/

#endif
  
