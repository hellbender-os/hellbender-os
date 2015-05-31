#ifndef _SIGNAL_H_
#define _SIGNAL_H_

struct timespec; // forward declaration.
#include <sys/types.h> // nonconforming: for pid_t, pthread_attr_t, pthread_t, size_t, uid_t.
#include <time.h> // nonconforming: struct timespec

// The Open Group Base Specifications Issue 7

#include <time.h>

#define SIG_DFL ((void (*)(int))1)
//Request for default signal handling.

#define SIG_ERR ((void (*)(int))2)
//Return value from signal() in case of error.

#define SIG_HOLD ((void (*)(int))3)
//[OB XSI] [Option Start] Request that signal be held. [Option End]

#define SIG_IGN ((void (*)(int))4)
// Request that signal be ignored.

typedef volatile int sig_atomic_t;
//Possibly volatile-qualified integer type of an object that can be accessed as an atomic entity, even in the presence of asynchronous interrupts.

typedef struct {
} sigset_t;
// Integer or structure type of an object used to represent sets of signals.

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

#define SIGEV_NONE 1
// No asynchronous notification is delivered when the event of interest occurs.

#define SIGEV_SIGNAL 2
// A queued signal, with an application-defined value, is generated when the event of interest occurs.

#define SIGEV_THREAD 3
// A notification function is called to perform notification.

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
*/
#define SIGABRT 1
//A
// Process abort signal.

#define SIGALRM 2
//T
// Alarm clock.

#define SIGBUS 3
//A
// Access to an undefined portion of a memory object.

#define SIGCHLD 4
//I
// Child process terminated, stopped, or continued.

#define SIGCONT 5
//C
// Continue executing, if stopped.

#define SIGFPE 6
//A
// Erroneous arithmetic operation.

#define SIGHUP 7
//T
// Hangup.

#define SIGILL 8
//A
// Illegal instruction.

#define SIGINT 9
//T
// Terminal interrupt signal.

#define SIGKILL 10
//T
// Kill (cannot be caught or ignored).

#define SIGPIPE 11
//T
// Write on a pipe with no one to read it.

#define SIGQUIT 12
//A
// Terminal quit signal.

#define SIGSEGV 13
//A
// Invalid memory reference.

#define SIGSTOP 14
//S
// Stop executing (cannot be caught or ignored).

#define SIGTERM 15
//T
// Termination signal.

#define SIGTSTP 16
//S
// Terminal stop signal.

#define SIGTTIN 17
//S
// Background process attempting read.

#define SIGTTOU 18
//S
// Background process attempting write.

#define SIGUSR1 20
//T
// User-defined signal 1.

#define SIGUSR2 21
//T
// User-defined signal 2.

#define SIGPOLL 30
//T
// Pollable event. [Option End]

#define SIGPROF 31
//T
// Profiling timer expired. [Option End]

#define SIGSYS 32
//A
// Bad system call.

#define SIGTRAP 33
//A
// Trace/breakpoint trap. [Option End]

#define SIGURG 34
//I
// High bandwidth data is available at a socket.

#define SIGVTALRM 35
//T
// Virtual timer expired.

#define SIGXCPU 36
//A
// CPU time limit exceeded.

#define SIGXFSZ 37
//A
// File size limit exceeded. [Option End]

/*
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

//[CX] [Option Start] The storage occupied by sa_handler and sa_sigaction may overlap, and a conforming application shall not use both simultaneously. [Option End]

#define SIG_BLOCK 1
// [CX] [Option Start] The resulting set is the union of the current set and the signal set pointed to by the argument set. [Option End]

#define SIG_UNBLOCK 2
// [CX] [Option Start] The resulting set is the intersection of the current set and the complement of the signal set pointed to by the argument set. [Option End]

#define SIG_SETMASK 3
// [CX] [Option Start] The resulting set is the signal set pointed to by the argument set. [Option End]

#define SA_NOCLDSTOP 1
// [CX] [Option Start] Do not generate SIGCHLD when children stop [Option End]
// [XSI] [Option Start]  or stopped children continue. [Option End]

#define SA_ONSTACK 2
// [XSI] [Option Start] Causes signal delivery to occur on an alternate stack. [Option End]

#define SA_RESETHAND 3
// [CX] [Option Start] Causes signal dispositions to be set to SIG_DFL on entry to signal handlers. [Option End]

#define SA_RESTART 4
// [CX] [Option Start] Causes certain functions to become restartable. [Option End]

#define SA_SIGINFO 5
// [CX] [Option Start] Causes extra information to be passed to signal handlers at the time of receipt of a signal. [Option End]

#define SA_NOCLDWAIT 6
// [CX] [Option Start] Causes implementations not to create zombie processes on child death. [Option End]

#define SA_NODEFER 7
// [CX] [Option Start] Causes signal not to be automatically blocked on entry to signal handler. [Option End]

#define SS_ONSTACK 8
// [XSI] [Option Start] Process is executing on an alternate signal stack. [Option End]

#define SS_DISABLE 9
// [XSI] [Option Start] Alternate signal stack is disabled. [Option End]

#define MINSIGSTKSZ 10
// [XSI] [Option Start] Minimum stack size for a signal handler. [Option End]

#define SIGSTKSZ 11
// [XSI] [Option Start] Default size in bytes for the alternate signal stack. [Option End]

typedef unsigned mcontext_t;

typedef struct {
  void     *ss_sp; //       Stack base or pointer. 
  size_t    ss_size; //     Stack size. 
  int       ss_flags; //    Flags. 
} stack_t;

typedef struct _ucontext_t ucontext_t;
typedef struct _ucontext_t {
  ucontext_t *uc_link; //     Pointer to the context that is resumed 
                       //     when this context returns. 
  sigset_t    uc_sigmask; //  The set of signals that are blocked when this 
                          //  context is active. 
  stack_t     uc_stack; //    The stack used by this context. 
  mcontext_t  uc_mcontext; // A machine-specific representation of the saved 
                           // context. 
} ucontext_t;

#define ILL_ILLOPC 1
// Illegal opcode.

#define ILL_ILLOPN 2
// Illegal operand.

#define ILL_ILLADR 3
// Illegal addressing mode.

#define ILL_ILLTRP 4
// Illegal trap.

#define ILL_PRVOPC 5
// Privileged opcode.

#define ILL_PRVREG 6
// Privileged register.

#define ILL_COPROC 7
// Coprocessor error.

#define ILL_BADSTK 8
// Internal stack error.

#define FPE_INTDIV 1
// Integer divide by zero.

#define FPE_INTOVF 2
// Integer overflow.

#define FPE_FLTDIV 3
// Floating-point divide by zero.

#define FPE_FLTOVF 4
// Floating-point overflow.

#define FPE_FLTUND 5
// Floating-point underflow.

#define FPE_FLTRES 6
// Floating-point inexact result.

#define FPE_FLTINV 7
// Invalid floating-point operation.

#define FPE_FLTSUB 8
// Subscript out of range.

#define SEGV_MAPERR 1
// Address not mapped to object.

#define SEGV_ACCERR 2
// Invalid permissions for mapped object.

#define BUS_ADRALN 1
// Invalid address alignment.

#define BUS_ADRERR 2
// Nonexistent physical address.

#define BUS_OBJERR 3
//Object-specific hardware error. [Option End]

#define TRAP_BRKPT 1
// Process breakpoint.

#define TRAP_TRACE 2
// Process trace trap. [Option End]

#define CLD_EXITED 1
// Child has exited.

#define CLD_KILLED 2
// Child has terminated abnormally and did not create a core file.

#define CLD_DUMPED 3
// Child has terminated abnormally and created a core file.

#define CLD_TRAPPED 4
// Traced child has trapped.

#define CLD_STOPPED 5
// Child has stopped.

#define CLD_CONTINUED 6
// Stopped child has continued. [Option End]

#define POLL_IN 1
// Data input available.

#define POLL_OUT 2
// Output buffers available.

#define POLL_MSG 3
// Input message available.

#define POLL_ERR 4
// I/O error.

#define POLL_PRI 5
// High priority input available.

#define POLL_HUP 6
// Device disconnected. [Option End]

#define SI_USER 1
// Signal sent by kill().

#define SI_QUEUE 2
// Signal sent by sigqueue().

#define SI_TIMER 3
// Signal generated by expiration of a timer set by timer_settime().

#define SI_ASYNCIO 4
// Signal generated by completion of an asynchronous I/O request

#define SI_MESGQ 5
// Signal generated by arrival of a message on an empty message queue [Option End]

//SIGILL, SIGFPE => void * si_addr // Address of faulting instruction.
//SIGSEGV, SIGBUS => void * si_addr // Address of faulting memory reference.
//SIGCHLD => pid_t si_pid // Child process ID.
//           int si_status // Exit value or signal.
//           uid_t si_uid // Real user ID of the process that sent the signal. [Option End]
//SIGPOLL => long si_band // Band event for POLL_IN, POLL_OUT, or POLL_MSG. [Option End]

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
