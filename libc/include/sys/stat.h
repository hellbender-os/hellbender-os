#ifndef _SYS_STAT_H
#define _SYS_STAT_H

#include <time.h> // nonconforming: for struct timespec
#include <sys/types.h> // nonconforming: blkcnt_t, blksize_t, dev_t, ino_t, mode_t, nlink_t, uid_t, gid_t, off_t, and time_t

// The Open Group Base Specifications Issue 7

struct stat {
  dev_t st_dev; //            Device ID of device containing file. 
  ino_t st_ino; //            File serial number. 
  mode_t st_mode; //          Mode of file (see below). 
  nlink_t st_nlink; //        Number of hard links to the file. 
  uid_t st_uid; //            User ID of file. 
  gid_t st_gid; //            Group ID of file. 
  dev_t st_rdev; //           Device ID (if file is character or block special). 
  off_t st_size; //           For regular files, the file size in bytes. 
                 //           For symbolic links, the length in bytes of the 
                 //           pathname contained in the symbolic link. 
                 //           For a shared memory object, the length in bytes. 
                 //           For a typed memory object, the length in bytes. 
                 //           For other file types, the use of this field is unspecified. 
  struct timespec st_atim; // Last data access timestamp. 
  struct timespec st_mtim; // Last data modification timestamp. 
  struct timespec st_ctim; // Last file status change timestamp.

  // Nonconforming: from issue 6
  time_t    st_atime; //   Time of last access. 
  time_t    st_mtime; //   Time of last data modification. 
  time_t    st_ctime; //   Time of last status change. 
  
  blksize_t st_blksize; //    A file system-specific preferred I/O block size 
                        //    for this object. In some file system types, this 
                        //    may vary from file to file. 
  blkcnt_t st_blocks; //      Number of blocks allocated for this object. 
};
// The st_ino and st_dev fields taken together uniquely identify the file within the system.

// For compatibility with earlier versions of this standard, the st_atime macro shall be defined with the value st_atim.tv_sec. Similarly, st_ctime and st_mtime shall be defined as macros with the values st_ctim.tv_sec and st_mtim.tv_sec, respectively.

#define S_IFMT   0xf000
//[XSI] [Option Start] Type of file.

#define S_IFBLK  0x6000
// Block special.

#define S_IFCHR  0x2000
// Character special.

#define S_IFIFO  0x1000
// FIFO special.

#define S_IFREG  0x8000
// Regular.

#define S_IFDIR  0x4000
// Directory.

#define S_IFLNK  0xa000
// Symbolic link.

#define S_IFSOCK 0xc000
// Socket.

#define S_IRWXU  00700
#define S_IRUSR  00400
#define S_IWUSR  00200
#define S_IXUSR  00100
// Read, write, execute/search by owner.
// Read permission, owner.
// Write permission, owner.
// Execute/search permission, owner.

#define S_IRWXG  00070
#define S_IRGRP  00040
#define S_IWGRP  00020
#define S_IXGRP  00010
// Read, write, execute/search by group.
// Read permission, group.
// Write permission, group.
// Execute/search permission, group.

#define S_IRWXO  00007
#define S_IROTH  00004
#define S_IWOTH  00002
#define S_IXOTH  00001
// Read, write, execute/search by others.
// Read permission, others.
// Write permission, others.
// Execute/search permission, others.

#define S_ISUID  04000
#define S_ISGID  02000
#define S_ISVTX  01000
// Set-user-ID on execution.
// Set-group-ID on execution.
// On directories, restricted deletion flag.

#define S_ISBLK(m)  ((m & S_IFMT) == S_IFBLK)
// Test for a block special file.

#define S_ISCHR(m)  ((m & S_IFMT) == S_IFCHR)
// Test for a character special file.

#define S_ISDIR(m)  ((m & S_IFMT) == S_IFDIR)
// Test for a directory.

#define S_ISFIFO(m) ((m & S_IFMT) == S_IFIFO)
// Test for a pipe or FIFO special file.

#define S_ISREG(m)  ((m & S_IFMT) == S_IFREG)
// Test for a regular file.

#define S_ISLNK(m)  ((m & S_IFMT) == S_IFLNK)
// Test for a symbolic link.

#define S_ISSOCK(m) ((m & S_IFMT) == S_IFSOCK)
// Test for a socket.

/*
The implementation may implement message queues, semaphores, or shared memory objects as distinct file types. The following macros shall be provided to test whether a file is of the specified type. The value of the buf argument supplied to the macros is a pointer to a stat structure. The macro shall evaluate to a non-zero value if the specified object is implemented as a distinct file type and the specified file type is contained in the stat structure referenced by buf. Otherwise, the macro shall evaluate to zero.

S_TYPEISMQ(buf)
Test for a message queue.
S_TYPEISSEM(buf)
Test for a semaphore.
S_TYPEISSHM(buf)
Test for a shared memory object.
*/

/*
[TYM] [Option Start] The implementation may implement typed memory objects as distinct file types, and the following macro shall test whether a file is of the specified type. The value of the buf argument supplied to the macros is a pointer to a stat structure. The macro shall evaluate to a non-zero value if the specified object is implemented as a distinct file type and the specified file type is contained in the stat structure referenced by buf. Otherwise, the macro shall evaluate to zero.

S_TYPEISTMO(buf)
Test macro for a typed memory object.
*/

/*
The <sys/stat.h> header shall define the following symbolic constants as distinct integer values outside of the range [0,999999999], for use with the futimens() and utimensat() functions: UTIME_NOW UTIME_OMIT
*/

int    chmod(const char *, mode_t);
int    fchmod(int, mode_t);
int    fchmodat(int, const char *, mode_t, int);
int    fstat(int, struct stat *);
int    fstatat(int, const char *, struct stat *, int);
int    futimens(int, const struct timespec [2]);
int    lstat(const char *, struct stat *);
int    mkdir(const char *, mode_t);
int    mkdirat(int, const char *, mode_t);
int    mkfifo(const char *, mode_t);
int    mkfifoat(int, const char *, mode_t);
int    mknod(const char *, mode_t, dev_t);
int    mknodat(int, const char *, mode_t, dev_t);
int    stat(const char *, struct stat *);
mode_t umask(mode_t);
int    utimensat(int, const char *, const struct timespec [2], int);

#endif
