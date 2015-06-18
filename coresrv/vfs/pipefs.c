#include <coresrv/pipefs.h>
#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

static struct vfs_pipefs pipefs;

void vfs_pipefs_init() {
  memset(&pipefs, 0, sizeof(struct vfs_pipefs));
  pipefs.filesys = (struct vfs_filesys) {
    .create    = NULL,
    .open      = NULL,
    .close     = MAKE_IDC_PTR(vfs_close, vfs_pipefs_close),
    .read      = MAKE_IDC_PTR(vfs_read, vfs_pipefs_read),
    .write     = MAKE_IDC_PTR(vfs_write, vfs_pipefs_write),
    .lseek     = NULL,
    .fsync     = NULL, // MAKE_IDC_PTR(vfs_fsync, vfs_devfs_fsync),
    .ftruncate = NULL,
    .fstat     = NULL,
    .link      = NULL,
    .unlink    = NULL,
    .termios   = NULL,
    .internal  = &pipefs,
  };
}

__IDCIMPL__ int vfs_pipe(IDC_PTR, struct vfs_file* f1, struct vfs_file* f2) {
  f1->filesys = pipefs.filesys;
  f2->filesys = pipefs.filesys;

  struct vfs_pipefs_buffer* buffer =
    (struct vfs_pipefs_buffer*)malloc(sizeof(struct vfs_pipefs_buffer));
  if (!buffer) {
    errno = ENOMEM;
    return -1;
  }
  buffer->mutex = PTHREAD_MUTEX_INITIALIZER;
  buffer->cond = PTHREAD_COND_INITIALIZER;
  buffer->size = PIPE_BUF;
  buffer->offset = 0;
  buffer->amount = 0;
  buffer->readers = 1;
  buffer->writers = 1;
  buffer->next = pipefs.first;
  pipefs.first = buffer;

  f1->stat.st_mode = S_IFIFO | S_IRUSR;
  f1->internal = buffer;
  f2->stat.st_mode = S_IFIFO | S_IWUSR;
  f2->internal = buffer;
  return 0;
}

__IDCIMPL__ int vfs_pipefs_close(IDC_PTR, struct vfs_file* file) {
  struct vfs_pipefs_buffer* buffer =
    (struct vfs_pipefs_buffer*)file->internal;
  pthread_mutex_lock(&buffer->mutex);

  memset(&file->filesys, 0, sizeof(file->filesys));
  if (file->stat.st_mode & S_IRUSR) --buffer->readers;
  if (file->stat.st_mode & S_IWUSR) --buffer->writers;
  file->stat.st_mode = 0;
  if (buffer && !buffer->readers && !buffer->writers) {
    free(buffer);
  }
  file->internal = NULL;

  pthread_mutex_unlock(&buffer->mutex);
  return 0;
}

__IDCIMPL__ ssize_t vfs_pipefs_read(IDC_PTR, struct vfs_file* file, void *data, size_t size) {
  if (!size) return 0;
  struct vfs_pipefs_buffer* buffer =
    (struct vfs_pipefs_buffer*)file->internal;
  pthread_mutex_lock(&buffer->mutex);

  // waiting for data
  while (buffer->writers && !(file->oflags & O_NONBLOCK) && !buffer->amount) {
    pthread_cond_wait(&buffer->cond, &buffer->mutex);
  }
  if (size > buffer->amount) size = buffer->amount;

  // copy the data
  uint8_t *udata = (uint8_t*)data;
  for (size_t i = 0; i < size; ++i) {
    udata[i] = buffer->data[(buffer->offset + i) % buffer->size];
  }
  buffer->offset = (buffer->offset + size) % buffer->size;
  buffer->amount -= size;

  pthread_cond_broadcast(&buffer->cond);
  pthread_mutex_unlock(&buffer->mutex);

  if ((file->oflags & O_NONBLOCK) && !size) {
    errno = EAGAIN;
    return -1;
  } else {
    return size;
  }
}

__IDCIMPL__ ssize_t vfs_pipefs_write(IDC_PTR, struct vfs_file* file, void *data, size_t size) {
  if (!size) return 0;
  struct vfs_pipefs_buffer* buffer =
    (struct vfs_pipefs_buffer*)file->internal;
  pthread_mutex_lock(&buffer->mutex);

  int retval = 0;
  while (retval != -1 && size) {
    // waiting until there is room for the data
    size_t room = buffer->size - buffer->amount;
    if (file->oflags & O_NONBLOCK) {
      if ((size <= PIPE_BUF && room < size) || !room) {
        errno = EAGAIN;
        retval = -1;
      } else if (size > room) size = room;
    } else {
      while (room < PIPE_BUF && room < size) {
        pthread_cond_wait(&buffer->cond, &buffer->mutex);
        room = buffer->size - buffer->amount;
      }
    }
    if (retval != -1) {
      // copy the data
      if (room > size) room = size;
      uint8_t *udata = (uint8_t*)data;
      for (size_t i = 0; i < room; ++i) {
        buffer->data[(buffer->offset + i) % buffer->size] = udata[i];
      }
      buffer->amount += room;
      size -= room;
      retval += room;
    }
  }
  
  pthread_cond_broadcast(&buffer->cond);
  pthread_mutex_unlock(&buffer->mutex);
  return retval;
}
