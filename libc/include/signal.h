#ifndef _SIGNAL_H_
#define _SIGNAL_H_

struct timespec; // forward declaration.
#include <sys/types.h> // nonconforming: for pid_t, pthread_attr_t

// The Open Group Base Specifications Issue 7

#include <time.h>

/*
The <signal.h> header shall define the following macros, which shall expand to constant expressions with distinct values that have a type compatible with the second argument to, and the return value of, the signal() function, and whose values shall compare unequal to the address of any declarable function.

SIG_DFL
Request for default signal handling.
SIG_ERR
Return value from signal() in case of error.
SIG_HOLD
[OB XSI] [Option Start] Request that signal be held. [Option End]
SIG_IGN
Request that signal be ignored.
[CX] [Option Start] The <signal.h> header shall define the pthread_t, size_t, and uid_t types as described in <sys/types.h>.

The <signal.h> header shall define the timespec structure as described in <time.h>. [Option End]

*/

typedef volatile int sig_atomic_t;
//Possibly volatile-qualified integer type of an object that can be accessed as an atomic entity, even in the presence of asynchronous interrupts.

typedef struct {
} sigset_t;
// Integer or structure type of an object used to represent sets of signals.

// pid_t As described in <sys/types.h>. 
// pthread_attr_t type as described in <sys/types.h>.

union sigval {
  int    sival_int; //    Integer signal value. 
  void  *sival_ptr; //    Pointer signal value. 
};

struct sigevent {
  int              sigev_notify; //            Notification type. 
  int              sigev_signo; //             Signal number. 
  union sigval     sigev_value; //             Signal value. 
  void           (*sigev_notify_function)(union sigval); //     Notification function. 
  pthread_attr_t *sigev_notify_attributes; //  Notification attributes. 
};

/*
The <signal.h> header shall define the following symbolic constants for the values of sigev_notify:

SIGEV_NONE
No asynchronous notification is delivered when the event of interest occurs.
SIGEV_SIGNAL
A queued signal, with an application-defined value, is generated when the event of interest occurs.
SIGEV_THREAD
A notification function is called to perform notification.
*/

/*
The <signal.h> header shall declare the SIGRTMIN and SIGRTMAX macros, which shall expand to positive integer expressions with type int, but which need not be constant expressions. These macros specify a range of signal numbers that are reserved for application use and for which the realtime signal behavior specified in this volume of POSIX.1-2008 is supported. The signal numbers in this range do not overlap any of the signals specified in the following table.

The range SIGRTMIN through SIGRTMAX inclusive shall include at least {RTSIG_MAX} signal numbers.

It is implementation-defined whether realtime signal behavior is supported for other signals. [Option End]

The <signal.h> header shall define the following macros that are used to refer to the signals that occur in the system. Signals defined here begin with the letters SIG followed by an uppercase letter. The macros shall expand to positive integer constant expressions with type int and distinct values. The value 0 is reserved for use as the null signal (see kill()). Additional implementation-defined signals may occur in the system.

[CX] [Option Start] The ISO C standard only requires the signal names SIGABRT, SIGFPE, SIGILL, SIGINT, SIGSEGV, and SIGTERM to be defined. [Option End]

The following signals shall be supported on all implementations (default actions are explained below the table):

Signal

Default Action

Description

SIGABRT

A

Process abort signal.

SIGALRM

T

Alarm clock.

SIGBUS

A

Access to an undefined portion of a memory object.

SIGCHLD

I

Child process terminated, stopped,

[XSI] [Option Start]  

 

or continued. [Option End]

SIGCONT

C

Continue executing, if stopped.

SIGFPE

A

Erroneous arithmetic operation.

SIGHUP

T

Hangup.

SIGILL

A

Illegal instruction.

SIGINT

T

Terminal interrupt signal.

SIGKILL

T

Kill (cannot be caught or ignored).

SIGPIPE

T

Write on a pipe with no one to read it.

SIGQUIT

A

Terminal quit signal.

SIGSEGV

A

Invalid memory reference.

SIGSTOP

S

Stop executing (cannot be caught or ignored).

SIGTERM

T

Termination signal.

SIGTSTP

S

Terminal stop signal.

SIGTTIN

S

Background process attempting read.

SIGTTOU

S

Background process attempting write.

SIGUSR1

T

User-defined signal 1.

SIGUSR2

T

User-defined signal 2.

[OB XSR] [Option Start] SIGPOLL

T

Pollable event. [Option End]

[OB XSI] [Option Start] SIGPROF

T

Profiling timer expired. [Option End]

[XSI] [Option Start] SIGSYS

A

Bad system call.

SIGTRAP

A

Trace/breakpoint trap. [Option End]

SIGURG

I

High bandwidth data is available at a socket.

[XSI] [Option Start] SIGVTALRM

T

Virtual timer expired.

SIGXCPU

A

CPU time limit exceeded.

SIGXFSZ

A

File size limit exceeded. [Option End]


The default actions are as follows:

T
Abnormal termination of the process.
A
Abnormal termination of the process [XSI] [Option Start]  with additional actions. [Option End]
I
Ignore the signal.
S
Stop the process.
C
Continue the process, if it is stopped; otherwise, ignore the signal.
The effects on the process in each case are described in XSH Signal Actions.
*/

typedef struct {
  int           si_signo; //  Signal number. 
  int           si_code; //   Signal code. 
  int           si_errno; //  If non-zero, an errno value associated with this signal, as described in <errno.h>. 
  pid_t         si_pid; //    Sending process ID. 
  uid_t         si_uid; //    Real user ID of sending process. 
  void         *si_addr; //   Address of faulting instruction. 
  int           si_status; // Exit value or signal. 
  long          si_band; //   Band event for SIGPOLL. 
  union sigval  si_value; //  Signal value. 
} siginfo_t;

struct sigaction {
  void   (*sa_handler)(int); //  Pointer to a signal-catching function or one of the SIG_IGN or SIG_DFL. 
  sigset_t sa_mask; //           Set of signals to be blocked during execution of the signal handling function. 
  int      sa_flags; //          Special flags. 
  void   (*sa_sigaction)(int, siginfo_t *, void *); // Pointer to a signal-catching function.
};
/*
[Option End]
[CX] [Option Start] The storage occupied by sa_handler and sa_sigaction may overlap, and a conforming application shall not use both simultaneously. [Option End]

The <signal.h> header shall define the following macros which shall expand to integer constant expressions that need not be usable in #if preprocessing directives:

SIG_BLOCK
[CX] [Option Start] The resulting set is the union of the current set and the signal set pointed to by the argument set. [Option End]
SIG_UNBLOCK
[CX] [Option Start] The resulting set is the intersection of the current set and the complement of the signal set pointed to by the argument set. [Option End]
SIG_SETMASK
[CX] [Option Start] The resulting set is the signal set pointed to by the argument set. [Option End]
The <signal.h> header shall also define the following symbolic constants:

SA_NOCLDSTOP
[CX] [Option Start] Do not generate SIGCHLD when children stop [Option End]
[XSI] [Option Start]  or stopped children continue. [Option End]
SA_ONSTACK
[XSI] [Option Start] Causes signal delivery to occur on an alternate stack. [Option End]
SA_RESETHAND
[CX] [Option Start] Causes signal dispositions to be set to SIG_DFL on entry to signal handlers. [Option End]
SA_RESTART
[CX] [Option Start] Causes certain functions to become restartable. [Option End]
SA_SIGINFO
[CX] [Option Start] Causes extra information to be passed to signal handlers at the time of receipt of a signal. [Option End]
SA_NOCLDWAIT
[CX] [Option Start] Causes implementations not to create zombie processes on child death. [Option End]
SA_NODEFER
[CX] [Option Start] Causes signal not to be automatically blocked on entry to signal handler. [Option End]
SS_ONSTACK
[XSI] [Option Start] Process is executing on an alternate signal stack. [Option End]
SS_DISABLE
[XSI] [Option Start] Alternate signal stack is disabled. [Option End]
MINSIGSTKSZ
[XSI] [Option Start] Minimum stack size for a signal handler. [Option End]
SIGSTKSZ
[XSI] [Option Start] Default size in bytes for the alternate signal stack. [Option End]
[CX] [Option Start] The <signal.h> header shall define the mcontext_t type through typedef. [Option End]

[CX] [Option Start] The <signal.h> header shall define the ucontext_t type as a structure that shall include at least the following members:

ucontext_t *uc_link     Pointer to the context that is resumed 
                        when this context returns. 
sigset_t    uc_sigmask  The set of signals that are blocked when this 
                        context is active. 
stack_t     uc_stack    The stack used by this context. 
mcontext_t  uc_mcontext A machine-specific representation of the saved 
                        context. 
*/
typedef struct {
  void     *ss_sp; //       Stack base or pointer. 
  size_t    ss_size; //     Stack size. 
  int       ss_flags; //    Flags. 
} stack_t;
/*
[CX] [Option Start] The <signal.h> header shall define the symbolic constants in the Code column of the following table for use as values of si_code that are signal-specific or non-signal-specific reasons why the signal was generated. [Option End]

Signal

Code

Reason

[CX] [Option Start] SIGILL

ILL_ILLOPC

Illegal opcode.

 

ILL_ILLOPN

Illegal operand.

 

ILL_ILLADR

Illegal addressing mode.

 

ILL_ILLTRP

Illegal trap.

 

ILL_PRVOPC

Privileged opcode.

 

ILL_PRVREG

Privileged register.

 

ILL_COPROC

Coprocessor error.

 

ILL_BADSTK

Internal stack error.

SIGFPE

FPE_INTDIV

Integer divide by zero.

 

FPE_INTOVF

Integer overflow.

 

FPE_FLTDIV

Floating-point divide by zero.

 

FPE_FLTOVF

Floating-point overflow.

 

FPE_FLTUND

Floating-point underflow.

 

FPE_FLTRES

Floating-point inexact result.

 

FPE_FLTINV

Invalid floating-point operation.

 

FPE_FLTSUB

Subscript out of range.

SIGSEGV

SEGV_MAPERR

Address not mapped to object.

 

SEGV_ACCERR

Invalid permissions for mapped object.

SIGBUS

BUS_ADRALN

Invalid address alignment.

 

BUS_ADRERR

Nonexistent physical address.

 

BUS_OBJERR

Object-specific hardware error. [Option End]

[XSI] [Option Start] SIGTRAP

TRAP_BRKPT

Process breakpoint.

 

TRAP_TRACE

Process trace trap. [Option End]

[CX] [Option Start] SIGCHLD

CLD_EXITED

Child has exited.

 

CLD_KILLED

Child has terminated abnormally and did not create a core file.

 

CLD_DUMPED

Child has terminated abnormally and created a core file.

 

CLD_TRAPPED

Traced child has trapped.

 

CLD_STOPPED

Child has stopped.

 

CLD_CONTINUED

Stopped child has continued. [Option End]

[OB XSR] [Option Start] SIGPOLL

POLL_IN

Data input available.

 

POLL_OUT

Output buffers available.

 

POLL_MSG

Input message available.

 

POLL_ERR

I/O error.

 

POLL_PRI

High priority input available.

 

POLL_HUP

Device disconnected. [Option End]

[CX] [Option Start] Any

SI_USER

Signal sent by kill().

 

SI_QUEUE

Signal sent by sigqueue().

 

SI_TIMER

Signal generated by expiration of a timer set by timer_settime().

 

SI_ASYNCIO

Signal generated by completion of an asynchronous I/O request

 

SI_MESGQ

Signal generated by arrival of a message on an empty message queue [Option End]

[CX] [Option Start] Implementations may support additional si_code values not included in this list, may generate values included in this list under circumstances other than those described in this list, and may contain extensions or limitations that prevent some values from being generated. Implementations do not generate a different value from the ones described in this list for circumstances described in this list. [Option End]
[CX] [Option Start] In addition, the following signal-specific information shall be available:

Signal

Member

Value

SIGILL, SIGFPE

void * si_addr

Address of faulting instruction.

SIGSEGV, SIGBUS

void * si_addr

Address of faulting memory reference.

SIGCHLD

pid_t si_pid

Child process ID.

 

int si_status

Exit value or signal.

 

uid_t si_uid

Real user ID of the process that sent the signal. [Option End]

[OB XSR] [Option Start] SIGPOLL

long si_band

Band event for POLL_IN, POLL_OUT, or POLL_MSG. [Option End]

For some implementations, the value of si_addr may be inaccurate.

The following shall be declared as functions and may also be defined as macros. Function prototypes shall be provided.
*/

int    kill(pid_t, int);
int    killpg(pid_t, int);
void   psiginfo(const siginfo_t *, const char *);
void   psignal(int, const char *);
int    pthread_kill(pthread_t, int);
int    pthread_sigmask(int, const sigset_t *,
           sigset_t *);
int    raise(int);
int    sigaction(int, const struct sigaction *,
           struct sigaction *);
int    sigaddset(sigset_t *, int);
int    sigaltstack(const stack_t *, stack_t *);
int    sigdelset(sigset_t *, int);
int    sigemptyset(sigset_t *);
int    sigfillset(sigset_t *);
int    sighold(int);
int    sigignore(int);
int    siginterrupt(int, int);
int    sigismember(const sigset_t *, int);
void (*signal(int, void (*)(int)))(int);
int    sigpause(int);
int    sigpending(sigset_t *);
int    sigprocmask(int, const sigset_t *, sigset_t *);
int    sigqueue(pid_t, int, const union sigval);
int    sigrelse(int);
void (*sigset(int, void (*)(int)))(int);
int    sigsuspend(const sigset_t *);
int    sigtimedwait(const sigset_t *, siginfo_t *,
           const struct timespec *);
int    sigwait(const sigset_t *, int *);
int    sigwaitinfo(const sigset_t *, siginfo_t *);

#endif
