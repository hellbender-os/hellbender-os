#include <string.h>
#include <signal.h>
#include <errno.h>

#define E(nr, str) case nr: return str;

char *strsignal(int signum) {
  switch (signum) {
    E(SIGABRT, "Process abort signal.");
    E(SIGALRM, "Alarm clock.");
    E(SIGBUS, "Access to an undefined portion of a memory object.");
    E(SIGCHLD, "Child process terminated, stopped, or continued.");
    E(SIGCONT, "Continue executing, if stopped.");
    E(SIGFPE, "Erroneous arithmetic operation.");
    E(SIGHUP, "Hangup.");
    E(SIGILL, "Illegal instruction.");
    E(SIGINT, "Terminal interrupt signal.");
    E(SIGKILL, "Kill (cannot be caught or ignored).");
    E(SIGPIPE, "Write on a pipe with no one to read it.");
    E(SIGQUIT, "Terminal quit signal.");
    E(SIGSEGV, "Invalid memory reference.");
    E(SIGSTOP, "Stop executing (cannot be caught or ignored).");
    E(SIGTERM, "Termination signal.");
    E(SIGTSTP, "Terminal stop signal.");
    E(SIGTTIN, "Background process attempting read.");
    E(SIGTTOU, "Background process attempting write.");
    E(SIGUSR1, "User-defined signal 1.");
    E(SIGUSR2, "User-defined signal 2.");
    E(SIGPOLL, "Pollable event.");
    E(SIGPROF, "Profiling timer expired.");
    E(SIGSYS, "Bad system call.");
    E(SIGTRAP, "Trace/breakpoint trap.");
    E(SIGURG, "High bandwidth data is available at a socket.");
    E(SIGVTALRM, "Virtual timer expired.");
    E(SIGXCPU, "CPU time limit exceeded.");
    E(SIGXFSZ, "File size limit exceeded.");
  default:
    errno = EINVAL;
    return "Unknown signal.";
  }
}

