// this file was automatically generated by make_coresrv_idc.sh

int __IDC__coresrv_kbd_getc(IDC_PTR);
typedef void* coresrv_kbd_getc_ptr;

int __IDC__vfs_close(IDC_PTR, struct vfs_file*);
typedef void* vfs_close_ptr;

int __IDC__vfs_fsync(IDC_PTR, struct vfs_file*);
typedef void* vfs_fsync_ptr;

int __IDC__vfs_ftruncate(IDC_PTR, struct vfs_file*, off_t);
typedef void* vfs_ftruncate_ptr;

int __IDC__vfs_mount(IDC_PTR, const char*, struct vfs_filesys *);
typedef void* vfs_mount_ptr;

int __IDC__vfs_open(IDC_PTR, struct vfs_file*, const char *, int);
typedef void* vfs_open_ptr;

int __IDC__vfs_unmount(IDC_PTR, const char*);
typedef void* vfs_unmount_ptr;

off_t __IDC__vfs_lseek(IDC_PTR, struct vfs_file*, off_t, int);
typedef void* vfs_lseek_ptr;

ssize_t __IDC__vfs_read(IDC_PTR, struct vfs_file*, void *, size_t);
typedef void* vfs_read_ptr;

ssize_t __IDC__vfs_write(IDC_PTR, struct vfs_file*, const void *, size_t);
typedef void* vfs_write_ptr;

unsigned __IDC__coresrv_rtc_ticks(IDC_PTR);
typedef void* coresrv_rtc_ticks_ptr;

void __IDC__coresrv_rtc_beep(IDC_PTR, unsigned freq, unsigned ticks);
typedef void* coresrv_rtc_beep_ptr;

void __IDC__coresrv_vga_putc(IDC_PTR, unsigned x, unsigned y, unsigned cursorx, unsigned c, unsigned color);
typedef void* coresrv_vga_putc_ptr;

extern uintptr_t __IDE__coresrv_kbd_getc;
extern uintptr_t __IDE__coresrv_rtc_beep;
extern uintptr_t __IDE__coresrv_rtc_ticks;
extern uintptr_t __IDE__coresrv_vga_putc;
extern uintptr_t __IDE__vfs_initfs_close;
extern uintptr_t __IDE__vfs_initfs_lseek;
extern uintptr_t __IDE__vfs_initfs_open;
extern uintptr_t __IDE__vfs_initfs_read;
extern uintptr_t __IDE__vfs_mount;
extern uintptr_t __IDE__vfs_open;
extern uintptr_t __IDE__vfs_rootfs_close;
extern uintptr_t __IDE__vfs_rootfs_lseek;
extern uintptr_t __IDE__vfs_rootfs_open;
extern uintptr_t __IDE__vfs_rootfs_read;
