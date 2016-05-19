HOST?=$(shell ../default-host.sh)
HOSTARCH:=$(shell ../target-triplet-to-arch.sh $(HOST))

CFLAGS?=
CPPFLAGS?=
LDFLAGS?=
LIBS?=

DESTDIR?=$(SYSROOT)
PREFIX?=/usr/local
EXEC_PREFIX?=$(PREFIX)
INCLUDEDIR?=$(PREFIX)/include
LIBDIR?=$(EXEC_PREFIX)/lib

CFLAGS:=$(CFLAGS) -std=gnu11 -mcmodel=large -Wall -Wextra -Werror -fno-exceptions -fno-unwind-tables -fno-asynchronous-unwind-tables
# -s
CPPFLAGS:=$(CPPFLAGS) -D__is_hellbender_libc -Iinclude -Isrc

CRT_OBJS:=\
startfiles/crt0S.o \
startfiles/crtiS.o \
startfiles/crtnS.o \

OBJS:=\
src/libc.o \
src/libc_init.o \
src/heap.o \
src/syscall.o \
src/vga.o \
src/rbtree.o \
src/dict.o \
src/io.o \
ctype/isspace.o \
ctype/tolower.o \
dirent/closedir.o \
dirent/dirfd.o \
dirent/opendir.o \
dirent/readdir.o \
dirent/rewinddir.o \
dirent/scandir.o \
dirent/seekdir.o \
dirent/telldir.o \
fcntl/creat.o \
fcntl/fcntl.o \
fcntl/fcntl_impl.o \
fcntl/open.o \
fnmatch/fnmatch.o \
pthread/mutex.o \
pthread/rwlock.o \
stdlib/abort.o \
stdlib/abs.o \
stdlib/atexit.o \
stdlib/atoi.o \
stdlib/bsearch.o \
stdlib/calloc.o \
stdlib/div.o \
stdlib/env.o \
stdlib/exit.o \
stdlib/free.o \
stdlib/itoa.o \
stdlib/labs.o \
stdlib/ldiv.o \
stdlib/lltoa.o \
stdlib/malloc.o \
stdlib/qsort.o \
stdlib/rand48.o \
stdlib/rand.o \
stdlib/realloc.o \
stdlib/realpath.o \
stdlib/strtod.o \
stdlib/strtol.o \
stdlib/system.o \
stdlib/ulltoa.o \
stdlib/utoa.o \
stdio/clearerr.o \
stdio/fclose.o \
stdio/ferror.o \
stdio/fflush.o \
stdio/fgetc.o \
stdio/fgets.o \
stdio/fileno.o \
stdio/fopen.o \
stdio/fprintf.o \
stdio/fputc.o \
stdio/fputs.o \
stdio/fread.o \
stdio/freopen.o \
stdio/fscanf.o \
stdio/fseek.o \
stdio/ftell.o \
stdio/fwrite.o \
stdio/getchar.o \
stdio/printf.o \
stdio/putc.o \
stdio/putchar.o \
stdio/puts.o \
stdio/scanf.o \
stdio/setbuf.o \
stdio/sprintf.o \
stdio/sscanf.o \
stdio/stdio_impl.o \
string/memccpy.o \
string/memchr.o \
string/memcmp.o \
string/memcpy.o \
string/memmove.o \
string/memset.o \
string/stpcpy.o \
string/stpncpy.o \
string/strcasecmp.o \
string/strcat.o \
string/strcat_s.o \
string/strchr.o \
string/strcmp.o \
string/strcoll.o \
string/strcpy.o \
string/strcpy_s.o \
string/strcspn.o \
string/strdup.o \
string/strerror.o \
string/strlen.o \
string/strncat.o \
string/strncmp.o \
string/strncpy.o \
string/strndup.o \
string/strnlen.o \
string/strpbrk.o \
string/strrchr.o \
string/strsignal.o \
string/strspn.o \
string/strstr.o \
string/strtok.o \
string/strxfrm.o \
sys/time/gettimeofday.o \
sys/uname.o \
sys/resource/rlimit.o \
sys/stat/chmod.o \
sys/stat/fstat.o \
sys/stat/futimens.o \
sys/stat/mkdir.o \
sys/stat/mkfifo.o \
sys/stat/mknod.o \
sys/stat/umask.o \
termios/tcgetattr.o \
termios/tcsetattr.o \
unistd/access.o \
unistd/chdir.o \
unistd/chown.o \
unistd/close.o \
unistd/dup.o \
unistd/getcwd.o \
unistd/getgid.o \
unistd/getgroups.o \
unistd/getopt.o \
unistd/getpid.o \
unistd/getuid.o \
unistd/isatty.o \
unistd/link.o \
unistd/lseek.o \
unistd/pipe.o \
unistd/read.o \
unistd/readlink.o \
unistd/rmdir.o \
unistd/sleep.o \
unistd/symlink.o \
unistd/sync.o \
unistd/sysconf.o \
unistd/truncate.o \
unistd/ttyname.o \
unistd/unlink.o \
unistd/write.o \


S_OBJS := $(OBJS:.o=S.o)

BINARIES=libc.a libcG.a libcS.a libcSG.a $(CRT_OBJS)

all: $(BINARIES)

.PHONY: all clean install install-headers

libc.a: $(OBJS)
	$(AR) rcs $@ $(OBJS)
	strip -g $@

libcG.a: $(OBJS)
	$(AR) rcs $@ $(OBJS)

libcS.a: $(S_OBJS)
	$(AR) rcs $@ $(S_OBJS)
	strip -g $@

libcSG.a: $(S_OBJS)
	$(AR) rcs $@ $(S_OBJS)

%S.o: %.c
	$(CC) -c $< -o $@ -fPIC $(CFLAGS) $(CPPFLAGS)

%S.o: %.S src/struct2asm.inc src/syscall.inc
	$(CC) -c $< -o $@ -fPIC $(CFLAGS) $(CPPFLAGS) -DASM

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS) $(CPPFLAGS)

%.o: %.S src/struct2asm.inc src/syscall.inc
	$(CC) -c $< -o $@ $(CFLAGS) $(CPPFLAGS) -DASM

src/struct2asm.inc: src/struct2asm.c
	$(CC) -S $< -o - | grep '^->' | awk '($$1 == "->") { print "#define " $$2 " " substr($$3,2) }' > $@

src/syscall.inc: $(DESTDIR)$(INCLUDEDIR)/hellbender/syscall.h src/syscall.S
	echo "// Automagically generated my Makefile, do not edit." > $@
	grep SYSCALL_INT $< >> $@
	grep " syscall_" $< | perl -ne '++$$i;/^uint64_t\s+([^(]+)/; print "\n.global $$1\n$$1:\n  mov \$$($$i-1), %rax\n  int \$$SYSCALL_INT\n  ret\n";' >> $@

clean:
	rm -f src/syscall.inc $(BINARIES) $(CRT_OBJS) $(OBJS) $(S_OBJS)

install: $(BINARIES)
	mkdir -p $(DESTDIR)$(LIBDIR)
	cp $(BINARIES) $(DESTDIR)$(LIBDIR)

install-headers:
	mkdir -p $(DESTDIR)$(INCLUDEDIR)
	cp -RT include $(DESTDIR)$(INCLUDEDIR)
