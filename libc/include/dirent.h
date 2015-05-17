#ifndef _DIRENT_H
#define _DIRENT_H

#include <limits.h>
#include <sys/types.h>

struct dirent {
  ino_t  d_ino;
  char   d_name[NAME_MAX+1];
};

#endif
