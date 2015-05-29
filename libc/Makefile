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

CFLAGS:=$(CFLAGS) -s -fno-exceptions -fno-unwind-tables -fno-asynchronous-unwind-tables -Wall -Wextra -Werror -fno-builtin
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
stdio/sprintf.o \
stdio/fprintf.o \
stdio/printf.o \
stdio/fputc.o \
stdio/puts.o \
stdio/putchar.o \
stdio/getchar.o \
stdio/fgetc.o \
stdio/fread.o \
stdio/fseek.o \
stdio/ftell.o \
stdio/fopen.o \
stdio/fclose.o \
stdio/setbuf.o \
stdlib/abort.o \
stdlib/itoa.o \
stdlib/utoa.o \
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
string/memcmp.o \
string/memcpy.o \
string/memmove.o \
string/memset.o \
string/strlen.o \
string/strcmp.o \
string/strncmp.o \
string/strcat.o \
string/strcat_s.o \
string/strcpy.o \
string/strcpy_s.o \
string/strrchr.o \
string/strdup.o \
string/strerror.o \
string/strchr.o \
time/localtime.o \
time/time.o \
ctype/tolower.o \
ctype/isspace.o \
sys/wait/waitpid.o \
heap/heap_init.o \
heap/heap_grow.o \
heap/heap_free.o \
heap/heap_malloc.o \
heap/heap_realloc.o \
hellbender/libc_init_basic.o \
hellbender/threadlocal.o \

HOSTEDOBJS:=\
$(ARCH_HOSTEDOBJS) \
fcntl/fcntl_init.o \
fcntl/open.o \
unistd/close.o \
unistd/fsync.o \
unistd/ftruncate.o \
unistd/lseek.o \
unistd/read.o \
unistd/write.o \
unistd/write.o \
unistd/dup2.o \
unistd/sleep.o \
unistd/getopt.o \
stdio/stdio_init.o \
stdio/fflush.o \
stdlib/exit.o \
stdlib/system.o \
stdlib/getenv.o \
sched/sched_yield.o \
keymap/create.o \
keymap/code2char.o \
keymap/keymap_fi.o \
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
	rm -f $(BINARIES) $(OBJS) $(ARCH_INITOBJS) $(LIBK_OBJS)
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
