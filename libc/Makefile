HOST?=$(shell ../default-host.sh)
HOSTARCH:=$(shell ../target-triplet-to-arch.sh $(HOST))

CFLAGS?=
CPPFLAGS?=
LDFLAGS?=
LIBS?=

DESTDIR?=
PREFIX?=/usr/local
EXEC_PREFIX?=$(PREFIX)
INCLUDEDIR?=$(PREFIX)/include
LIBDIR?=$(EXEC_PREFIX)/lib

CFLAGS:=$(CFLAGS) -Wall -Wextra -Werror
# -s -fno-exceptions -fno-unwind-tables -fno-asynchronous-unwind-tables
CPPFLAGS:=$(CPPFLAGS) -D__is_hellbender_libc -Iinclude
LIBK_CFLAGS:=$(CFLAGS) -ffreestanding
LIBK_CPPFLAGS:=$(CPPFLAGS) -D__is_hellbender_kernel

ARCHDIR:=arch/$(HOSTARCH)

include $(ARCHDIR)/make.config

CFLAGS:=$(CFLAGS) $(ARCH_CFLAGS)
CPFLAGS:=$(CPPFLAGS) $(ARCH_CPPFLAGS)
LIBK_CFLAGS:=$(LIBK_CFLAGS) $(KERNEL_ARCH_CFLAGS)
LIBK_CPFLAGS:=$(LIBK_CPPFLAGS) $(KERNEL_ARCH_CPPFLAGS)

FREEOBJS:=\
$(ARCH_FREEOBJS) \
libgen/basename.o \
libgen/dirname.o \
stdio/sprintf.o \
stdio/fprintf.o \
stdio/fputc.o \
stdio/printf.o \
stdio/puts.o \
stdio/putchar.o \
stdio/fwrite.o \
stdlib/abort.o \
stdlib/itoa.o \
stdlib/lltoa.o \
stdlib/utoa.o \
stdlib/ulltoa.o \
stdlib/calloc.o \
stdlib/free.o \
stdlib/malloc.o \
stdlib/realloc.o \
stdlib/labs.o \
stdlib/abs.o \
stdlib/ldiv.o \
stdlib/div.o \
stdlib/qsort.o \
stdlib/bsearch.o \
stdlib/strtol.o \
stdlib/atoi.o \
string/memmove.o \
string/memchr.o \
string/memcmp.o \
string/memcpy.o \
string/memccpy.o \
string/memset.o \
string/strlen.o \
string/strnlen.o \
string/strcoll.o \
string/strerror.o \
string/strsignal.o \
string/strtok.o \
string/strxfrm.o \
string/strcmp.o \
string/strncmp.o \
string/strcat.o \
string/strcat_s.o \
string/strncat.o \
string/strcpy.o \
string/strcpy_s.o \
string/strncpy.o \
string/strrchr.o \
string/strdup.o \
string/strndup.o \
string/strchr.o \
string/strcspn.o \
string/strspn.o \
string/strstr.o \
string/strpbrk.o \
string/stpcpy.o \
string/stpncpy.o \
strings/strcasecmp.o \
strings/strncasecmp.o \
ctype/tolower.o \
ctype/isspace.o \
hellbender/libc_init_basic.o \
hellbender/threadlocal.o \
hellbender/linkedlist.o \
hellbender/heap_init.o \
hellbender/heap_grow.o \
hellbender/heap_free.o \
hellbender/heap_malloc.o \
hellbender/heap_realloc.o \

HOSTEDOBJS:=\
$(ARCH_HOSTEDOBJS) \
fcntl/fcntl_init.o \
fcntl/fcntl.o \
fcntl/open.o \
fcntl/openat.o \
fcntl/creat.o \
unistd/close.o \
unistd/fsync.o \
unistd/ftruncate.o \
unistd/truncate.o \
unistd/lseek.o \
unistd/read.o \
unistd/write.o \
unistd/write.o \
unistd/dup2.o \
unistd/sleep.o \
unistd/getopt.o \
unistd/getcwd.o \
unistd/chdir.o \
unistd/fchdir.o \
unistd/isatty.o \
unistd/link.o \
unistd/unlink.o \
unistd/rmdir.o \
unistd/access.o \
stdio/stdio_init.o \
stdio/clearerr.o \
stdio/getchar.o \
stdio/fputs.o \
stdio/fgetc.o \
stdio/fgets.o \
stdio/fread.o \
stdio/fseek.o \
stdio/ftell.o \
stdio/fopen.o \
stdio/freopen.o \
stdio/fclose.o \
stdio/setbuf.o \
stdio/fflush.o \
stdio/fwrite.o \
stdio/fileno.o \
stdio/ferror.o \
stdio/scanf.o \
stdio/sscanf.o \
stdio/fscanf.o \
stdlib/exit.o \
stdlib/system.o \
stdlib/getenv.o \
sched/sched_yield.o \
time/localtime.o \
time/time.o \
sys/stat/fstat.o \
sys/wait/waitpid.o \
sys/ioctl.o \
spawn/posix_spawn.o \
semaphore/sem_init.o \
semaphore/sem_open.o \
semaphore/sem_post.o \
semaphore/sem_wait.o \
pthread/cond_init.o \
pthread/cond_broadcast.o \
pthread/cond_signal.o \
pthread/cond_wait.o \
pthread/mutex_init.o \
pthread/mutex_lock.o \
pthread/mutex_unlock.o \
termios/tcgetattr.o \
termios/tcsetattr.o \
poll/poll.o \
signal/signal.o \
keymap/create.o \
keymap/code2char.o \
keymap/keymap_fi.o \
hellbender/libc_init_advanced.o \
../kernel/syscall/syscall_stub.o \

CORE_STUB:=../coresrv/coresrv/coresrv_stub.o
CORE_SYMS:=../coresrv/coresrv/coresrv_syms.o

OBJS:=\
$(FREEOBJS) \
$(HOSTEDOBJS) \
$(CORE_STUB) \

LIBK_OBJS:=$(FREEOBJS:.o=.libk.o)

MATH_OBJS:=\
math/fabs.o \

BINARIES=libc.a libg.a libm.a

all: $(BINARIES)

.PHONY: all clean install install-headers install-libs

# libcore is EXACTLY like libc, except it doesn't include __IDE__* symbols
# that are defined in coresrv, which is not yet built..
# libcore will be used to build coresrv, which will generate CORE_SYMS,
# and then we can build the final libc.
libcore.a: $(OBJS)
	$(AR) rcs $@ $(OBJS)

libc.a: $(OBJS) $(CORE_SYMS)
	$(AR) rcs $@ $(OBJS) $(CORE_SYMS)

libg.a:
	$(AR) rcs $@

libk.a: $(LIBK_OBJS)
	$(AR) rcs $@ $(LIBK_OBJS)

libm.a: $(MATH_OBJS)
	$(AR) rcs $@ $(MATH_OBJS)

%.o: %.c
	$(CC) -c $< -o $@ -std=gnu11 $(CFLAGS) $(CPPFLAGS)

%.o: %.S
	$(CC) -c $< -o $@ $(CFLAGS) $(CPPFLAGS)

%.libk.o: %.c
	$(CC) -c $< -o $@ -std=gnu11 $(LIBK_CFLAGS) $(LIBK_CPPFLAGS)

%.libk.o: %.S
	$(CC) -c $< -o $@ $(LIBK_CFLAGS) $(LIBK_CPPFLAGS)

clean:
	rm -f $(BINARIES) libk.a libcore.a $(OBJS) $(ARCH_INITOBJS) $(LIBK_OBJS)
#	rm -f *.o */*.o */*/*.o *~ */*~ */*/*~

install: install-headers install-libs

install-headers:
	mkdir -p $(DESTDIR)$(INCLUDEDIR)
	cp -RT include $(DESTDIR)$(INCLUDEDIR)

install-libs: $(BINARIES)
	mkdir -p $(DESTDIR)$(LIBDIR)
	cp $(BINARIES) $(DESTDIR)$(LIBDIR)

install-libk: libk.a libcore.a $(ARCH_INITOBJS)
	mkdir -p $(DESTDIR)$(LIBDIR)
	cp libk.a libcore.a $(DESTDIR)$(LIBDIR)
	cp $(ARCH_INITOBJS) $(DESTDIR)$(LIBDIR)
