#ifndef _SYS_WAIT_H
#define _SYS_WAIT_H

#include <sys/types.h> // nonconforming: id_t and pid_t

// bits forming wait status:
#define _SYS_WAIT_MASK     0xf00
#define _SYS_WAIT_EXITED   0x100
#define _SYS_WAIT_SIGNALED 0x200
#define _SYS_WAIT_STOPPED  0x400
#define _SYS_WAIT_CONTD    0x800

// The Open Group Base Specifications Issue 7

#include <signal.h>

#define WCONTINUED 1
// [XSI] [Option Start] Report status of continued child process. [Option End]

#define WNOHANG 2
// Do not hang if no status is available; return immediately.

#define WUNTRACED 4
// Report status of stopped child process.

#define WIFEXITED(status) ((status & _SYS_WAIT_MASK) == _SYS_WAIT_EXITED)
// True if child exited normally.

#define WEXITSTATUS(status) (status & 0xff)
// Return exit status.

#define WIFSIGNALED(status) ((status & _SYS_WAIT_MASK) == _SYS_WAIT_SIGNALED)
// True if child exited due to uncaught signal.

#define WTERMSIG(status) (status & 0xff)
// Return signal number that caused process to terminate.

#define WIFSTOPPED(status) ((status & _SYS_WAIT_MASK) == _SYS_WAIT_STOPPED)
// True if child is currently stopped.

#define WSTOPSIG(status) (status & 0xff)
// Return signal number that caused process to stop.

#define WIFCONTINUED(status) ((status & _SYS_WAIT_MASK) == _SYS_WAIT_CONTD)
// [XSI] [Option Start] True if child has been continued. [Option End]

#define WEXITED 1
// Wait for processes that have exited.

#define WNOWAIT 2
// Keep the process whose status is returned in infop in a waitable state.

#define WSTOPPED 3
// Status is returned for any child that has stopped upon receipt of a signal.

typedef unsigned idtype_t;
#define P_ALL 1
#define P_PGID 2
#define P_PID 3

pid_t  wait(int *);
int    waitid(idtype_t, id_t, siginfo_t *, int);
pid_t  waitpid(pid_t, int *, int);

#endif
