#ifndef _SYS_UTSNAME_H
#define _SYS_UTSNAME_H

// nonconforming: name length in utsname struct.
#define _SYS_UTSNAME_LEN 32

//The Open Group Base Specifications Issue 7

struct utsname {
  char  sysname[_SYS_UTSNAME_LEN]; //  Name of this implementation of the operating system. 
  char  nodename[_SYS_UTSNAME_LEN]; // Name of this node within the communications network to which this node is attached, if any. 
  char  release[_SYS_UTSNAME_LEN]; //  Current release level of this implementation. 
  char  version[_SYS_UTSNAME_LEN]; //  Current version level of this release. 
  char  machine[_SYS_UTSNAME_LEN]; //  Name of the hardware type on which the system is running.
};

int uname(struct utsname *);

#endif
