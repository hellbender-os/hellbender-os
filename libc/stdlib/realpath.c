#include <stdlib.h>
#include <limits.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <coresrv/vfs.h>

char *realpath(const char *file_name, char *resolved_name) {
  if (strlen(file_name) >= PATH_MAX) {
    errno = ENAMETOOLONG;
    return NULL;
  }
  char *alloc_name = NULL;
  if (!*resolved_name) {
    alloc_name = (char*)malloc(PATH_MAX);
    resolved_name = alloc_name;
  }
  strcpy(resolved_name, file_name);
  // TODO: normalize name by removing dots.
  struct vfs_file file;
  if (CORE_IDC(vfs_resolve, NULL, &file, file_name, F_OK) != 0) {
    goto error;
  }
  if (!file.stat.st_mode) {
    errno = ENOENT;
    goto error;
  }
  strcpy(resolved_name, file.real_path);
  if (file.filesys.close) IDC(vfs_close, file.filesys.close, &file);
  return resolved_name;

 error:
  if (file.filesys.close) IDC(vfs_close, file.filesys.close, &file);
  free(alloc_name);
  return NULL;
}
