HOST?=$(shell ../default-host.sh)
HOSTARCH:=$(shell ../target-triplet-to-arch.sh $(HOST))

CFLAGS?=-O2 -g0
CPPFLAGS?=
LDFLAGS?=
LIBS?=

DESTDIR?=
PREFIX?=/usr/local
EXEC_PREFIX?=$(PREFIX)
INCLUDEDIR?=$(PREFIX)/include
LIBDIR?=$(EXEC_PREFIX)/lib

CFLAGS:=$(CFLAGS) -Wall -Wextra
CPPFLAGS:=$(CPPFLAGS) -D__is_hellbender_libc -Iinclude
LIBK_CFLAGS:=$(CFLAGS) -ffreestanding -fbuiltin
LIBK_CPPFLAGS:=$(CPPFLAGS) -D__is_hellbender_kernel

ARCHDIR:=arch/$(HOSTARCH)

include $(ARCHDIR)/make.config

CFLAGS:=$(CFLAGS) $(ARCH_CFLAGS)
CPFLAGS:=$(CPPFLAGS) $(ARCH_CPPFLAGS)
LIBK_CFLAGS:=$(LIBK_CFLAGS) $(KERNEL_ARCH_CFLAGS)
LIBK_CPFLAGS:=$(LIBK_CPPFLAGS) $(KERNEL_ARCH_CPPFLAGS)

FREEOBJS:=\
$(ARCH_FREEOBJS) \
fcntl/fcntl_init.o \
fcntl/open.o \
unistd/close.o \
unistd/fsync.o \
unistd/ftruncate.o \
unistd/lseek.o \
unistd/read.o \
unistd/write.o \
stdio/sprintf.o \
stdio/printf.o \
stdio/puts.o \
stdio/putchar.o \
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
stdlib/system.o \
stdlib/getenv.o \
stdlib/abort.o \
string/memcmp.o \
string/memcpy.o \
string/memmove.o \
string/memset.o \
string/strlen.o \
string/strcmp.o \
string/strcpy.o \
sched/sched_yield.o \
semaphore/sem_open.o \
semaphore/sem_post.o \
semaphore/sem_wait.o \
heap/heap_init.o \
heap/heap_grow.o \
heap/heap_free.o \
heap/heap_malloc.o \
heap/heap_realloc.o \
hellbender/libc_init.o \
hellbender/syscall.o \
keymap/create.o \
keymap/code2char.o \
keymap/keymap_fi.o \

HOSTEDOBJS:=\
$(ARCH_HOSTEDOBJS) \
stdlib/exit.o \

OBJS:=\
$(FREEOBJS) \
$(HOSTEDOBJS) \

LIBK_OBJS:=$(FREEOBJS:.o=.libk.o)

BINARIES=libc.a libg.a libk.a

all: $(BINARIES)

.PHONY: all clean install install-headers install-libs

libc.a: $(OBJS) $(ARCH_INITOBJS)
	$(AR) rcs $@ $(OBJS)

libg.a:
	$(AR) rcs $@

libk.a: $(LIBK_OBJS)
	$(AR) rcs $@ $(LIBK_OBJS)

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
	cp $(ARCH_INITOBJS) $(DESTDIR)$(LIBDIR)
