#ifndef _CORESRV_PIPEFS_H
#define _CORESRV_PIPEFS_H

#include <coresrv/vfs.h>
#include <limits.h>

struct vfs_pipefs {
  struct vfs_filesys filesys;

  struct vfs_pipefs_buffer {
    struct vfs_pipefs_buffer *next;
    volatile size_t readers;
    volatile size_t writers;
    volatile uint8_t data[PIPE_BUF]; // circular buffer for data.
    size_t size; // total size of buffer.
    volatile size_t offset; // beginning of data in the buffer.
    volatile size_t amount; // number of data bytes in the buffer.

    pthread_mutex_t mutex;
    pthread_cond_t cond;
  } *first;
};

struct vfs_pipefs_file {
  struct vfs_pipefs_buffer *buffer;
};

void vfs_pipefs_init();
__IDC__ int vfs_pipe(IDC_PTR, struct vfs_file*, struct vfs_file*);

int vfs_pipefs_close(IDC_PTR, struct vfs_file*);
ssize_t vfs_pipefs_read(IDC_PTR, struct vfs_file*, void *, size_t);
ssize_t vfs_pipefs_write(IDC_PTR, struct vfs_file*, void *, size_t);

#endif
