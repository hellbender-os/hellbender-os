#ifndef _UNISTD_H
#define _UNISTD_H

#include <sys/types.h>
#include <inttypes.h>


#define _POSIX_VERSION     200112L
#define _POSIX2_VERSION    200112L
#define _POSIX2_C_VERSION  -1
#define _XOPEN_VERSION     500
#define _XOPEN_XCU_VERSION -1

#define _POSIX_CHOWN_RESTRICTED  1
#define _POSIX_NO_TRUNC          1
#undef _POSIX_VDISABLE          
#undef _POSIX_SAVED_IDS         
#undef _POSIX_JOB_CONTROL       

#define _POSIX_THREADS               0
#define _POSIX_THREAD_ATTR_STACKADDR 0
#define _POSIX_THREAD_ATTR_STACKSIZE 0
#define _POSIX_THREAD_PROCESS_SHARED 0
#define _POSIX_THREAD_SAFE_FUNCTIONS 0

#define _POSIX2_C_BIND           1
#define _POSIX2_C_DEV           -1
#define _POSIX2_CHAR_TERM        1
#define _POSIX2_FORT_DEV        -1
#define _POSIX2_FORT_RUN        -1
#define _POSIX2_LOCALEDEF       -1
#define _POSIX2_SW_DEV          -1
#define _POSIX2_UPE             -1
#define _XOPEN_CRYPT            -1
#define _XOPEN_ENH_I18N         -1 // nonconformal
#define _XOPEN_LEGACY           -1
#define _XOPEN_REALTIME         -1
#define _XOPEN_REALTIME_THREADS -1
#define _XOPEN_SHM              -1 // nonconformal
#define _XBS5_ILP32_OFF32        1
#define _XBS5_ILP32_OFFBIG      -1
#define _XBS5_LP64_OFF64        -1
#define _XBS5_LPBIG_OFFBIG      -1

#define _POSIX_FSYNC             -1 // conconformal
#define _POSIX_MAPPED_FILES      -1 // conconformal
#define _POSIX_MEMORY_PROTECTION -1 // conconformal

#undef _POSIX_PRIORITIZED_IO

#define _POSIX_ASYNC_IO  -1
#define _POSIX_PRIO_IO   -1
#define _POSIX_SYNC_IO   -1

#define NULL ((void*)0)

#define R_OK  01
#define W_OK  02
#define X_OK  04
#define F_OK  08

/*
_CS_PATH
_CS_XBS5_ILP32_OFF32_CFLAGS
_CS_XBS5_ILP32_OFF32_LDFLAGS
_CS_XBS5_ILP32_OFF32_LIBS
_CS_XBS5_ILP32_OFF32_LINTFLAGS
_CS_XBS5_ILP32_OFFBIG_CFLAGS
_CS_XBS5_ILP32_OFFBIG_LDFLAGS
_CS_XBS5_ILP32_OFFBIG_LIBS
_CS_XBS5_ILP32_OFFBIG_LINTFLAGS
_CS_XBS5_LP64_OFF64_CFLAGS
_CS_XBS5_LP64_OFF64_LDFLAGS
_CS_XBS5_LP64_OFF64_LIBS
_CS_XBS5_LP64_OFF64_LINTFLAGS
_CS_XBS5_LPBIG_OFFBIG_CFLAGS
_CS_XBS5_LPBIG_OFFBIG_LDFLAGS
_CS_XBS5_LPBIG_OFFBIG_LIBS
_CS_XBS5_LPBIG_OFFBIG_LINTFLAGS
*/

#define SEEK_SET 1
#define SEEK_CUR 2
#define SEEK_END 3

/*
_SC_2_C_BIND
_SC_2_C_DEV
_SC_2_C_VERSION
_SC_2_FORT_DEV
_SC_2_FORT_RUN
_SC_2_LOCALEDEF
_SC_2_SW_DEV
_SC_2_UPE
_SC_2_VERSION
_SC_ARG_MAX
_SC_AIO_LISTIO_MAX
_SC_AIO_MAX
_SC_AIO_PRIO_DELTA_MAX
_SC_ASYNCHRONOUS_IO
_SC_ATEXIT_MAX
_SC_BC_BASE_MAX
_SC_BC_DIM_MAX
_SC_BC_SCALE_MAX
_SC_BC_STRING_MAX
_SC_CHILD_MAX
_SC_CLK_TCK
_SC_COLL_WEIGHTS_MAX
_SC_DELAYTIMER_MAX
_SC_EXPR_NEST_MAX
_SC_FSYNC
_SC_GETGR_R_SIZE_MAX
_SC_GETPW_R_SIZE_MAX
_SC_IOV_MAX
_SC_JOB_CONTROL
_SC_LINE_MAX
_SC_LOGIN_NAME_MAX
_SC_MAPPED_FILES
_SC_MEMLOCK
_SC_MEMLOCK_RANGE
_SC_MEMORY_PROTECTION
_SC_MESSAGE_PASSING
_SC_MQ_OPEN_MAX
_SC_MQ_PRIO_MAX
_SC_NGROUPS_MAX
_SC_OPEN_MAX
_SC_PAGESIZE
_SC_PAGE_SIZE
_SC_PASS_MAX (LEGACY)
_SC_PRIORITIZED_IO
_SC_PRIORITY_SCHEDULING
_SC_RE_DUP_MAX


_SC_REALTIME_SIGNALS
_SC_RTSIG_MAX
_SC_SAVED_IDS
_SC_SEMAPHORES
_SC_SEM_NSEMS_MAX
_SC_SEM_VALUE_MAX
_SC_SHARED_MEMORY_OBJECTS
_SC_SIGQUEUE_MAX
_SC_STREAM_MAX
_SC_SYNCHRONIZED_IO
_SC_THREADS
_SC_THREAD_ATTR_STACKADDR
_SC_THREAD_ATTR_STACKSIZE
_SC_THREAD_DESTRUCTOR_ITERATIONS
_SC_THREAD_KEYS_MAX
_SC_THREAD_PRIORITY_SCHEDULING
_SC_THREAD_PRIO_INHERIT
_SC_THREAD_PRIO_PROTECT
_SC_THREAD_PROCESS_SHARED
_SC_THREAD_SAFE_FUNCTIONS
_SC_THREAD_STACK_MIN
_SC_THREAD_THREADS_MAX
_SC_TIMERS
_SC_TIMER_MAX
_SC_TTY_NAME_MAX
_SC_TZNAME_MAX
_SC_VERSION
_SC_XOPEN_VERSION
_SC_XOPEN_CRYPT
_SC_XOPEN_ENH_I18N
_SC_XOPEN_SHM
_SC_XOPEN_UNIX
_SC_XOPEN_XCU_VERSION
_SC_XOPEN_LEGACY
_SC_XOPEN_REALTIME
_SC_XOPEN_REALTIME_THREADS
_SC_XBS5_ILP32_OFF32
_SC_XBS5_ILP32_OFFBIG
_SC_XBS5_LP64_OFF64
_SC_XBS5_LPBIG_OFFBIG
*/

#define _SC_PAGESIZE 4096
#define _SC_PAGE_SIZE 4096

#define F_LOCK   1
#define F_ULOCK  2
#define F_TEST   3
#define F_TLOCK  4

/*
#define _PC_ASYNC_IO
#define _PC_CHOWN_RESTRICTED
#define _PC_FILESIZEBITS
#define _PC_LINK_MAX
#define _PC_MAX_CANON         
#define _PC_MAX_INPUT
#define _PC_NAME_MAX
#define _PC_NO_TRUNC
#define _PC_PATH_MAX
#define _PC_PIPE_BUF
#define _PC_PRIO_IO
#define _PC_SYNC_IO
#define _PC_VDISABLE
*/

#define STDIN_FILENO  0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

int          access(const char *, int);
unsigned int alarm(unsigned int);
int          brk(void *);
int          chdir(const char *);
int          chown(const char *, uid_t, gid_t);
int          close(int);
size_t       confstr(int, char *, size_t);
char        *crypt(const char *, const char *);
char        *ctermid(char *);
int          dup(int);
int          dup2(int, int);
void         encrypt(char[64], int);
int          execl(const char *, const char *, ...);
int          execle(const char *, const char *, ...);
int          execlp(const char *, const char *, ...);
int          execv(const char *, char *const []);
int          execve(const char *, char *const [], char *const []);
int          execvp(const char *, char *const []);
void        _exit(int);
int          fchown(int, uid_t, gid_t);
int          fchdir(int);
int          fdatasync(int);
pid_t        fork(void);
long int     fpathconf(int, int);
int          fsync(int);
int          ftruncate(int, off_t);
char        *getcwd(char *, size_t);
gid_t        getegid(void);
uid_t        geteuid(void);
gid_t        getgid(void);
int          getgroups(int, gid_t []);
long         gethostid(void);
char        *getlogin(void);
int          getlogin_r(char *, size_t);
int          getopt(int, char * const [], const char *);
pid_t        getpgid(pid_t);
pid_t        getpgrp(void);
pid_t        getpid(void);
pid_t        getppid(void);
pid_t        getsid(pid_t);
uid_t        getuid(void);
char        *getwd(char *);
int          isatty(int);
int          lchown(const char *, uid_t, gid_t);
int          link(const char *, const char *);
int          lockf(int, int, off_t);
off_t        lseek(int, off_t, int);
int          nice(int);
long int     pathconf(const char *, int);
int          pause(void);
int          pipe(int [2]);
ssize_t      pread(int, void *, size_t, off_t);
int          pthread_atfork(void (*)(void), void (*)(void),void(*)(void));
ssize_t      pwrite(int, const void *, size_t, off_t);
ssize_t      read(int, void *, size_t);
int          readlink(const char *, char *, size_t);
int          rmdir(const char *);
void        *sbrk(intptr_t);
int          setgid(gid_t);
int          setpgid(pid_t, pid_t);
pid_t        setpgrp(void);
int          setregid(gid_t, gid_t);
int          setreuid(uid_t, uid_t);
pid_t        setsid(void);
int          setuid(uid_t);
unsigned int sleep(unsigned int);
void         swab(const void *, void *, ssize_t);
int          symlink(const char *, const char *);
void         sync(void);
long int     sysconf(int);
pid_t        tcgetpgrp(int);
int          tcsetpgrp(int, pid_t);
int          truncate(const char *, off_t);
char        *ttyname(int);
int          ttyname_r(int, char *, size_t);
useconds_t   ualarm(useconds_t, useconds_t);
int          unlink(const char *);
int          usleep(useconds_t);
pid_t        vfork(void);
ssize_t      write(int, const void *, size_t);

extern char   *optarg;
extern int    optind, opterr, optopt;

#endif
