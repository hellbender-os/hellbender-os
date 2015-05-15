#ifndef _FCNTL_H
#define _FCNTL_H

#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

/*
F_DUPFD
F_GETFD
F_SETFD
F_GETFL
F_SETFL
F_GETLK
F_SETLK
F_SETLKW
*/
/*
FD_CLOEXEC*
*/
/*
F_RDLCK
F_UNLCK
F_WRLCK
*/

#define O_CREAT   0x0001
#define O_EXCL    0x0002
#define O_NOCTTY  0x0004
#define O_TRUNC   0x0008

#define O_APPEND  0x0010
#define O_DSYNC   0x0020
#define O_NONBLCK 0x0040
#define O_RSYNC   0x0080
#define O_SYNC    0x0100

#define O_ACCMODE 0x0200

#define O_RDONLY  0x1000
#define O_RDWR    0x2000
#define O_WRONLY  0x4000

struct flock {
  short l_type;
  short l_whence;
  off_t l_start;
  off_t l_len;
  pid_t l_pid;
};

int creat(const char *, mode_t);
int fcntl(int, int, ...);
int open(const char *, int, ...);

#endif
