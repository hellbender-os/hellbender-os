#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>

#include <coresrv/vfs.h>
#include <coresrv/initfs.h>
#include <kernel/kernel.h>

// like strrchr but skips the last character (dir names end with separator).
static char *strrchr2(const char *str, int ch) {
  char *ptr = NULL;
  for (; *str && *(str+1); ++str) if (*str == ch) ptr = (char*)str;
  return ptr;
}

static struct vfs_initfs_entry* find_parent(const char *path,
                                           struct vfs_initfs_entry *ptr) {
  char *path_end = strrchr2(path, VFS_SEPARATOR_CHAR);
  if (!path_end) return NULL;
  size_t path_len = path_end - path;
  for (; ptr; ptr = ptr->flat_next) {
    if (strncmp(ptr->path, path, path_len) == 0) {
      return ptr;
    }
  }
  printf("Parent not found for %s\n", path);
  return NULL;
}

void vfs_initfs_init(struct vfs_initfs* initfs, uint8_t *buffer, size_t size) {
  memset(initfs, 0, sizeof(struct vfs_initfs));
  initfs->filesys.open = MAKE_IDC_PTR(vfs_open, vfs_initfs_open);
  initfs->filesys.close = MAKE_IDC_PTR(vfs_close, vfs_initfs_close);
  initfs->filesys.read = MAKE_IDC_PTR(vfs_read, vfs_initfs_read);
  //initfs->filesys.write = MAKE_IDC_PTR(vfs_write, vfs_initfs_write);
  initfs->filesys.lseek = MAKE_IDC_PTR(vfs_lseek, vfs_initfs_lseek);
  //initfs->filesys.fsync = MAKE_IDC_PTR(vfs_fsync, vfs_initfs_fsync);
  //initfs->filesys.ftruncate = MAKE_IDC_PTR(vfs_ftruncate, vfs_initfs_ftruncate);
  
  initfs->buffer = buffer;
  initfs->size = size;
  initfs->flat_first = &initfs->root;
  
  // scan the buffer, build a directory hierarchy.
  while (size > sizeof(struct vfs_initfs_header)) {
    struct vfs_initfs_header *header = (struct vfs_initfs_header*) buffer;
    if (header->magic != 070707) {
      printf("Invalid header.\n");
      break;
    }
    size_t filesize = ((unsigned)header->filesize_lo
                       + (((unsigned)header->filesize_hi)<<16));
    //printf("dev=%u, ino=%u, mode=%u, uid=%u, gid=%u, nlink=%u, "
    //       "rdev=%u, mtime=%u, namesize=%u, filesize=%u %u\n",
    //       header->dev, header->ino, header->mode, header->uid,
    //       header->gid, header->nlink, header->rdev,
    //       (unsigned)header->mtime_lo + (((unsigned)header->mtime_hi)<<16),
    //       header->namesize, header->filesize_hi, header->filesize_lo);
    size -= sizeof(struct vfs_initfs_header);
    buffer += sizeof(struct vfs_initfs_header);
    char *fullpath = (char*)buffer;
    //printf("Adding fullpath '%s'\n", fullpath);
    if (filesize == 0
        && header->mode == 0
        && strncmp(fullpath, "TRAILER!!", 9) == 0) break;
    buffer += header->namesize + (header->namesize&1);
    uint8_t *data = (uint8_t*)buffer;
    buffer += filesize + (filesize&1);
    struct vfs_initfs_entry *entry = NULL;
    struct vfs_initfs_entry *parent = NULL;
    if (strcmp(fullpath, "./") == 0) {
      entry = &initfs->root;
    } else {
      parent = find_parent(fullpath, initfs->flat_first);
      if (!parent) parent = &initfs->root;
      entry = calloc(sizeof(struct vfs_initfs_entry), 1);
      entry->flat_next = initfs->flat_first;
      initfs->flat_first = entry;
    }
    entry->header = *header;
    char *path_end = strrchr2(fullpath, VFS_SEPARATOR_CHAR);
    entry->name = path_end ? path_end + 1 : fullpath;
    entry->path = fullpath;
    entry->data = filesize ? data : NULL;
    entry->filesize = filesize;
    //printf("Added '%s' as '%s'/'%s'\n",
    //       fullpath, parent ? parent->name : "", entry->name);
    if (parent) {
      entry->next = parent->children;
      parent->children = entry;
    }
  }
}

__IDCIMPL__ int vfs_initfs_open(IDC_PTR, struct vfs_file* file, const char *name, int flags) {
  (void)(flags); // TODO: check flags.
  
  if (!file->internal) {
    file->internal = malloc(sizeof(struct vfs_initfs_file));
  }
  struct vfs_initfs_file *internal = (struct vfs_initfs_file*)(file->internal);
  struct vfs_initfs *initfs = (struct vfs_initfs *)file->filesys;

  int name_len = strlen(name);
  if (name_len == 0) {
    // root of the filesystem.
    internal->this = &initfs->root;
    internal->child = internal->this->children;
    internal->offset = 0;
    return 0;

  } else {
    // find children by name (cpio directories don't have the tailing slash).
    /*
    char purename[NAME_MAX+1];
    strcpy_s(purename, NAME_MAX+1, name);
    int name_is_dir = 0;
    if (purename[name_len-1] == VFS_SEPARATOR_CHAR) {
      name_is_dir = 1;
      purename[name_len-1] = 0;
    }
    */
    for (struct vfs_initfs_entry *ptr = internal->this->children; 
         ptr; ptr = ptr->next) {
      if (strcmp(ptr->name, name) == 0) {
        /*int ptr_is_dir = S_ISDIR(ptr->header.mode) ? 1 : 0;
          if (name_is_dir == ptr_is_dir) {*/
          internal->this = ptr;
          internal->child = internal->this->children;
          internal->offset = 0;
          return 0;
          //}
      }
    }
  }

  return -1;
}

__IDCIMPL__ int vfs_initfs_close(IDC_PTR, struct vfs_file* file) {
  file->filesys = NULL;
  free(file->internal);
  file->internal = NULL;
  return 0;
}

__IDCIMPL__ ssize_t vfs_initfs_read(IDC_PTR, struct vfs_file* file, void *buf, size_t size) {
  struct vfs_initfs_file *internal = (struct vfs_initfs_file*)(file->internal);

  if (S_ISDIR(internal->this->header.mode)) {
    // directories return dirent and advance to next children.
    if (size != sizeof(struct dirent)) {
      printf("Directories only support struct dirent.\n");
      return 0;
    }
    if (internal->child) {
      struct dirent* de = (struct dirent*) buf;
      de->d_ino = (ino_t)internal->child->header.ino;
      strcpy_s(de->d_name, NAME_MAX+1, internal->child->name);
      internal->child = internal->child->next;
      internal->offset += size;
      return size;
    } else {
      return 0;
    }
    
  } else if (internal->this->data) {
    // files return file data.
    size_t bytes = size;
    if (internal->offset + bytes > internal->this->filesize) {
      bytes = internal->this->filesize - internal->offset;
    }
    memcpy(buf, internal->this->data + internal->offset, bytes);
    internal->offset += bytes;
    return bytes;
  }
  return 0;
}

__IDCIMPL__ off_t vfs_initfs_lseek(IDC_PTR, struct vfs_file* file, off_t off, int where) {
  struct vfs_initfs_file *internal = (struct vfs_initfs_file*)(file->internal);

  switch (where) {
  case SEEK_SET:
    internal->offset = off;
    break;
  case SEEK_CUR:
    internal->offset += off;
    break;
  case SEEK_END:
    internal->offset = internal->this->filesize - off;
  }
  if (internal->offset < 0) internal->offset = 0;
  if (internal->offset > (int)internal->this->filesize) {
    internal->offset = internal->this->filesize;
  }

  // when seeking directories, find the corresponding children.
  if (S_ISDIR(internal->this->header.mode)) {
    off_t i = internal->offset;
    off_t j = 0;
    internal->child = internal->this->children;
    while (i >= (int)sizeof(struct dirent) && internal->child) {
      i -= sizeof(struct dirent);
      j += sizeof(struct dirent);
      internal->child = internal->child->next;
    }
    internal->offset = j;
  }

  return internal->offset;
}


