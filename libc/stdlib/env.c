#include <stdlib.h>
#include <string.h>
#include <errno.h>

char **environ;
char **_environ_top; // just past the allocated environ array.
char *_environ_static_top; // values below this cannot be reallocated.

static int _getenv(const char* name, size_t len) {
  for (int i = 0; environ[i]; ++i) {
    char *evar = environ[i];
    if (strncmp(evar, name, len) == 0 &&
        evar[len] == '=') {
      return i;
    }
  }
  return -1;
}

static int _appenv(char *string) {
  int i = 0;
  for (; environ + i < _environ_top; ++i) {
    if (!environ[i] || strlen(environ[i]) == 0) {
      environ[i] = string;
      return 0;
    }
  }
  int k = 2 * i + 32;
  size_t newsize = k * sizeof(char*);
  char **newenv = realloc(environ, newsize);
  if (!newenv) {
    errno = ENOMEM;
    return -1;
  }
  for (int j = i; j < k; ++j) {
    newenv[j] = 0;
  }
  environ = newenv;
  _environ_top = environ + k;
  environ[i] = string;
  return 0;
}

char* getenv (const char* name) {
  size_t name_len = strlen(name);
  int idx = _getenv(name, name_len);
  if (idx != -1) {
    char *ptr = environ[idx];
    return ptr + name_len + 1;
  } else {
    return NULL;
  }
}

int setenv(const char *envname, const char *envval, int overwrite) {
  size_t name_len = strlen(envname);
  if (name_len == 0 || strchr(envname, '=')) {
    errno = EINVAL;
    return -1;
  }
  size_t val_len = strlen(envval);
  size_t full_len = name_len + 1 + val_len + 1;
  int idx = _getenv(envname, name_len);
  
  if (idx == -1) { // create new env entry
    char *ptr = (char*)malloc(full_len);
    if (!ptr) {
      errno = ENOMEM;
      return -1;
    }
    memcpy(ptr, envname, name_len);
    ptr[name_len] = '=';
    memcpy(ptr + name_len + 1, envval, val_len + 1);
    return _appenv(ptr);

  } else if (overwrite) { // replace old entry
    char *ptr = environ[idx];
    if (ptr < _environ_static_top) {
      // cannot realloc entries mapped by the kernel.
      ptr = (char*)malloc(full_len);
      if (ptr) {
        memcpy(ptr, envname, name_len);
        ptr[name_len] = '=';
      }
    } else {
      ptr = realloc(environ[idx], full_len);
    }
    if (!ptr) {
      errno = ENOMEM;
      return -1;
    }
    memcpy(ptr + name_len + 1, envval, val_len + 1);
    environ[idx] = ptr;
  }
  return 0;
}

int unsetenv(const char *name) {
  size_t name_len = strlen(name);
  if (name_len == 0 || strchr(name, '=')) {
    errno = EINVAL;
    return -1;
  }
  int idx = _getenv(name, name_len);
  if (idx != -1) {
    if (environ[idx] >= _environ_static_top) {
      // cannot release entries mapped by kernel.
      free(environ[idx]);
    }
    environ[idx] = "";
  }
  return 0;
}
