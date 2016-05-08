#ifndef _DIRENT_H
#define _DIRENT_H

#include <sys/types.h> // nonconforming: ino_t type as described in <sys/types.h>.
#include <limits.h> // nonconforming: NAME_MAX

// The Open Group Base Specifications Issue 7

struct dirent {
  ino_t  d_ino; //       File serial number. 
  char   d_name[NAME_MAX+1]; //    Filename string of entry. 
  int    d_name_len; // strlen(d_name);
};

//The array d_name is of unspecified size, but shall contain a filename of at most {NAME_MAX} bytes followed by a terminating null byte.

typedef struct {
  int fd;
  struct dirent de;
} DIR;
//A type representing a directory stream. The DIR type may be an incomplete type.

int            alphasort(const struct dirent **, const struct dirent **);
int            closedir(DIR *);
int            dirfd(DIR *);
DIR           *fdopendir(int);
DIR           *opendir(const char *);
struct dirent *readdir(DIR *);
int            readdir_r(DIR *, struct dirent *,
                   struct dirent **);
void           rewinddir(DIR *);
int            scandir(const char *, struct dirent ***,
                   int (*)(const struct dirent *),
                   int (*)(const struct dirent **,
                   const struct dirent **));
void           seekdir(DIR *, long);
long           telldir(DIR *);

#endif
