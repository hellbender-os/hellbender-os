#ifndef _SYS_UN_H
#define _SYS_UN_H

#define _SYS_UN_SUN_PATH_LEN 65

#include <sys/socket.h> // nonconforming: sa_family_t

// The Open Group Base Specifications Issue 7

struct sockaddr_un {
  sa_family_t  sun_family; //  Address family. 
  char         sun_path[_SYS_UN_SUN_PATH_LEN]; //  Socket pathname. 
};
//The sockaddr_un structure is used to store addresses for UNIX domain sockets. Pointers to this type shall be cast by applications to struct sockaddr * for use with socket functions.  

#endif
