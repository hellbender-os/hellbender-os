#ifndef _SYS_WAIT_H
#define _SYS_WAIT_H

// The Open Group Base Specifications Issue 7

/*
The <sys/wait.h> header shall define the following symbolic constants for use with waitpid():

WCONTINUED
[XSI] [Option Start] Report status of continued child process. [Option End]
WNOHANG
Do not hang if no status is available; return immediately.
WUNTRACED
Report status of stopped child process.
The <sys/wait.h> header shall define the following macros for analysis of process status values:

WEXITSTATUS
Return exit status.
WIFCONTINUED
[XSI] [Option Start] True if child has been continued. [Option End]
WIFEXITED
True if child exited normally.
WIFSIGNALED
True if child exited due to uncaught signal.
WIFSTOPPED
True if child is currently stopped.
WSTOPSIG
Return signal number that caused process to stop.
WTERMSIG
Return signal number that caused process to terminate.
The <sys/wait.h> header shall define the following symbolic constants as possible values for the options argument to waitid():

WEXITED
Wait for processes that have exited.
WNOWAIT
Keep the process whose status is returned in infop in a waitable state.
WSTOPPED
Status is returned for any child that has stopped upon receipt of a signal.
The [XSI] [Option Start]  WCONTINUED [Option End] and WNOHANG constants, described above for waitpid(), can also be used with waitid().

The type idtype_t shall be defined as an enumeration type whose possible values shall include at least the following: P_ALL P_PGID P_PID

The <sys/wait.h> header shall define the id_t and pid_t types as described in <sys/types.h>.

The <sys/wait.h> header shall define the siginfo_t type as described in <signal.h>.

Inclusion of the <sys/wait.h> header may also make visible all symbols from <signal.h>.

The following shall be declared as functions and may also be defined as macros. Function prototypes shall be provided.

pid_t  wait(int *);
int    waitid(idtype_t, id_t, siginfo_t *, int);
pid_t  waitpid(pid_t, int *, int);
*/

#endif
