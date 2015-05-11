#ifndef _KERNEL_SEMAPHORE
#define _KERNEL_SEMAPHORE

// maximum length of semaphore name buffer (including trailing 0).
#define SEMAPHORE_MAX_NAME 256

typedef struct semaphore semaphore_t;

semaphore_t* semaphore_create(const char* name);
semaphore_t* semaphore_find(const char* name);
void semaphore_post(semaphore_t* s);
void semaphore_wait(semaphore_t* s);

#endif
