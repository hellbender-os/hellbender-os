#ifndef _STRING_H
#define _STRING_H 1

#include <sys/cdefs.h>

#include <stddef.h>
#include <errno.h>

#ifdef __cplusplus
extern "C" {
#endif

int memcmp(const void*, const void*, size_t);
void* memcpy(void* __restrict, const void* __restrict, size_t);
void* memmove(void*, const void*, size_t);
void* memset(void*, int, size_t);

size_t strlen(const char*);
int strcmp(const char*, const char*);
int strncmp(const char *str1, const char *str2, size_t num);
char* strcat(char* destination, const char* source);
char* strcpy(char* destination, const char* source);

char *strrchr(const char *str, int ch);
  
errno_t strcat_s(char* destination, size_t dest_size, const char* source);
errno_t strcpy_s(char* destination, size_t dest_size, const char* source);
  
#ifdef __cplusplus
}
#endif

#endif
