#ifndef __HELLBENDER_STDIO_H__
#define __HELLBENDER_STDIO_H__

#include <hellbender/list.h>

#define _FILE_BUFSIZE 4096

struct _FILE {
  list_item_t item;
  char *buffer;
  size_t buf_size;
  unsigned buf_bytes;
  unsigned buf_offset;
  int fd;
  int row; // Read Or Write: 0 == write, 1 == read.
  int eof;
  int error;
};

struct _stdio {
  list_t files;
  struct _FILE _stdin, _stdout, _stderr;
};

extern struct _stdio _stdio;

#endif
