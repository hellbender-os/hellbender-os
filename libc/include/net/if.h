#ifndef _NET_IF_H
#define _NET_IF_H

// The Open Group Base Specifications Issue 7

struct if_nameindex {
  unsigned  if_index; //  Numeric index of the interface. 
  char     *if_name; //   Null-terminated name of the interface. 
};

#define IF_NAMESIZE 65
/// Interface name length.

void                  if_freenameindex(struct if_nameindex *);
char                 *if_indextoname(unsigned, char *);
struct if_nameindex  *if_nameindex(void);
unsigned              if_nametoindex(const char *);

#endif
