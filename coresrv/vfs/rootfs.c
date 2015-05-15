#include <string.h>
#include <coresrv/vfs.h>

__IDCIMPL__ int coresrv_rootfs_open(IDC_PTR, vfo_t* vfo, const char *name, int flags) {
  memset(vfo, 0, sizeof(vfo_t));
  vfo->close = MAKE_IDC_PTR(coresrv_vfs_close, coresrv_rootfs_close);
  vfo->read = MAKE_IDC_PTR(coresrv_vfs_read, coresrv_rootfs_read);
  vfo->lseek = MAKE_IDC_PTR(coresrv_vfs_lseek, coresrv_rootfs_lseek);
  return 0;
}

__IDCIMPL__ int coresrv_rootfs_close(IDC_PTR, vfo_t* vto) {
  return -1;
}

__IDCIMPL__ ssize_t coresrv_rootfs_read(IDC_PTR, vfo_t* vto, void * buf, size_t size) {
  return 0;
}

__IDCIMPL__ off_t coresrv_rootfs_lseek(IDC_PTR, vfo_t* vto, off_t off, int where) {
  return 0;
}


