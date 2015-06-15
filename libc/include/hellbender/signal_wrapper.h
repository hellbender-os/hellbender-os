#ifndef _HELLBENDER_SIGNAL_WRAPPER_H
#define _HELLBENDER_SIGNAL_WRAPPER_H

#include <signal.h>

// ASM that saves the state, calls _signal_procesor, restores state, and irets.
void _signal_wrapper(int, siginfo_t*, void*, struct sigaction*);

// sets up signal mask, calls the signal function, restores signal mask.
void _signal_processor(int, siginfo_t*, void*, struct sigaction*);

#endif
