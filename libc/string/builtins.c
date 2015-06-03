#include <stddef.h>

void    *memchr(const void *s, int c, size_t n) {
  return __builtin_memchr(s, c, n);
}

int      memcmp(const void *ptr1, const void *ptr2, size_t num) {
  return __builtin_memcmp(ptr1, ptr2, num);
}

void    *memcpy(void *dest, const void *src, size_t num) {
  return __builtin_memcpy(dest, src, num);
}

void    *memset(void *ptr, int value, size_t num) {
  return __builtin_memset(ptr, value, num);
}

char    *stpcpy(char *dest, const char *src) {
  return __builtin_stpcpy(dest, src);
}

char    *stpncpy(char *dest, const char *src, size_t num) {
  return __builtin_stpncpy(dest, src, num);
}

char    *strcat(char *dest, const char *src) {
  return __builtin_strcat(dest, src);
}

char    *strchr(const char *str, int ch) {
  return __builtin_strchr(str, ch);
}

int      strcmp(const char *str1, const char *str2) {
  return __builtin_strcmp(str1, str2);
}

char    *strcpy(char *dest, const char *src) {
  return __builtin_strcpy(dest, src);
}

size_t   strcspn(const char *str1, const char *str2) {
  return __builtin_strcspn(str1, str2);
}

char    *strdup(const char *str) {
  return __builtin_strdup(str);
}

size_t   strlen(const char *str) {
  return __builtin_strlen(str);
}

char    *strncat(char *dest, const char *src, size_t num) {
  return __builtin_strncat(dest, src, num);
}

int      strncmp(const char *str1, const char *str2, size_t num) {
  return __builtin_strncmp(str1, str2, num);
}

char    *strncpy(char *dest, const char *src, size_t num) {
  return __builtin_strncpy(dest, src, num);
}

char    *strndup(const char *str, size_t num) {
  return __builtin_strndup(str, num);
}

char    *strpbrk(const char *str1, const char *str2) {
  return __builtin_strpbrk(str1, str2);
}

char    *strrchr(const char *str, int ch) {
  return __builtin_strrchr(str, ch);
}

size_t   strspn(const char *str1, const char *str2) {
  return __builtin_strspn(str1, str2);
}

char    *strstr(const char *str1, const char *str2) {
  return __builtin_strstr(str1, str2);
}
