#ifndef _SYS_STAT_H
#define _SYS_STAT_H

#include <sys/types.h>

#define S_IFMT   0xf
#define S_IFBLK  1
#define S_IFCHR  2
#define S_IFIFO  3
#define S_IFREG  4
#define S_IFDIR  5
#define S_IFLNK  6

#define S_IRWXU  00700
#define S_IRUSR  00100
#define S_IWUSR  00200 
#define S_IXUSR  00400

#define S_IRWXG  00070
#define S_IRGRP  00010
#define S_IWGRP  00020
#define S_IXGRP  00040

#define S_IRWXO  00007
#define S_IROTH  00001
#define S_IWOTH  00002
#define S_IXOTH  00004

#define S_ISUID  01000
#define S_ISGID  02000
#define S_ISVTX  04000

#define S_ISBLK(m)  ((m & S_IFMT) == S_IFBLK)
#define S_ISCHR(m)  ((m & S_IFMT) == S_IFDIR)
#define S_ISDIR(m)  ((m & S_IFMT) == S_IFIFO)
#define S_ISFIFO(m) ((m & S_IFMT) == S_IFREG)
#define S_ISREG(m)  ((m & S_IFMT) == S_IFREG)
#define S_ISLNK(m)  ((m & S_IFMT) == S_IFLNK)

#define S_TYPEISMQ(buf)  0
#define S_TYPEISSEM(buf) 0
#define S_TYPEISSHM(buf) 0
     
struct stat {
  dev_t     st_dev;
  ino_t     st_ino;
  mode_t    st_mode;
  nlink_t   st_nlink;
  uid_t     st_uid;
  gid_t     st_gid;
  dev_t     st_rdev;
  off_t     st_size;
  time_t    st_atime;
  time_t    st_mtime;
  time_t    st_ctime;
  blksize_t st_blksize;
  blkcnt_t  st_blocks;
};

#endif
