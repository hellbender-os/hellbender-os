#ifndef _PWD_H
#define _PWD_H

#include <sys/types.h> // nonconforming: gid_t, uid_t, size_t

// The Open Group Base Specifications Issue 7

struct passwd {
  char    *pw_name; //   User's login name. 
  uid_t    pw_uid; //    Numerical user ID. 
  gid_t    pw_gid; //    Numerical group ID. 
  char    *pw_dir; //    Initial working directory. 
  char    *pw_shell; //  Program to use as shell. 
};


void           endpwent(void);
struct passwd *getpwent(void);
struct passwd *getpwnam(const char *);
int            getpwnam_r(const char *, struct passwd *, char *,
                          size_t, struct passwd **);
struct passwd *getpwuid(uid_t);
int            getpwuid_r(uid_t, struct passwd *, char *,
                          size_t, struct passwd **);
void           setpwent(void);

#endif
