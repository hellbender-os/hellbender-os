#ifndef __HELLBENDER_FCNTL_H__
#define __HELLBENDER_FCNLT_H__

#include <limits.h>
#include <termios.h>
#include <sys/types.h>

#include <hellbender/inline.h>
#include <hellbender/fs/vfs.h>

#define HANDLE_GROUP 16

typedef struct _fd {
  int status; // 0 free, 1 allocated, -1 locked
  int flags; // flags for fcntl F_GETFD, F_SETFD
  vfs_file_t file;
  off_t offset;
} _fd_t;

struct _fcntl {
  _fd_t fildes[HANDLE_GROUP];
  _fd_t* indirect_fildes[OPEN_MAX];
  _fd_t cwd;
  mode_t umask;
};

extern struct _fcntl _fcntl;

int _fcntl_reserve_fildes(int min_fildes);
void _fcntl_release_fildes(int fildes);

_fd_t* _fcntl_get_fd2(int fildes);
_fd_t* _fcntl_peek_fd2(int fildes);

INLINE _fd_t* _fcntl_get_fd(int fildes) {
  if (fildes < HANDLE_GROUP) return _fcntl.fildes + fildes;
  else return _fcntl_get_fd2(fildes);
}

INLINE _fd_t* _fcntl_peek_fd(int fildes) {
  if (fildes < HANDLE_GROUP) return _fcntl.fildes + fildes;
  else return _fcntl_peek_fd2(fildes);
}

INLINE vfs_file_t* _fcntl_get_file(int fildes) {
  _fd_t *fd = _fcntl_peek_fd(fildes);
  return fd ? &fd->file : 0;
}

#endif
