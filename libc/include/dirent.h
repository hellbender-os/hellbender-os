#ifndef _DIRENT_H
#define _DIRENT_H

#include <limits.h>
#include <sys/types.h>

struct dirent {
  ino_t  d_ino;
  char   d_name[NAME_MAX];
};

struct dirent_stat {
  struct dirent d_dirent;
  struct stat d_stat;
};

#endif
