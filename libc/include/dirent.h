#ifndef _DIRENT_H
#define _DIRENT_H

// The Open Group Base Specifications Issue 7

typedef struct {
  int fd;
} DIR;
//A type representing a directory stream. The DIR type may be an incomplete type.

struct dirent {
  ino_t  d_ino; //       File serial number. 
  char   d_name[NAME_MAX+1]; //    Filename string of entry. 
};

// ino_t type as described in <sys/types.h>.
//The array d_name is of unspecified size, but shall contain a filename of at most {NAME_MAX} bytes followed by a terminating null byte.

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
