#ifndef __LIBC_PTHREAD_H__
#define __LIBC_PTHREAD_H__

// returns 1 if pthread has been linked as part of the application.
// if so, proper synchronization should be applied.
int _pthread_enabled();

#endif
