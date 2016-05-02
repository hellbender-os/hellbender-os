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

CFLAGS:=$(CFLAGS) -std=gnu11 -mcmodel=small -mno-mmx -mno-sse -mno-sse2 -Wall -Wextra -Werror -fno-exceptions -fno-unwind-tables -fno-asynchronous-unwind-tables
# -s
CPPFLAGS:=$(CPPFLAGS) -D__is_hellbender_libc -Iinclude -Isrc

CRT_OBJS:=\
startfiles/crt0.o \
startfiles/crti.o \
startfiles/crtn.o \

OBJS:=\
$(CRT_OBJS) \
src/libc.o \
src/libc_init.o \
src/heap.o \
src/mutex.o \
src/syscall.o \
src/vga.o \
src/rbtree.o \
src/dict.o \
stdlib/abs.o \
stdlib/atexit.o \
stdlib/atoi.o \
stdlib/bsearch.o \
stdlib/div.o \
stdlib/exit.o \
stdlib/free.o \
stdlib/itoa.o \
stdlib/labs.o \
stdlib/ldiv.o \
stdlib/lltoa.o \
stdlib/malloc.o \
stdlib/qsort.o \
stdlib/strtol.o \
stdlib/ulltoa.o \
stdlib/utoa.o \
string/memccpy.o \
string/memchr.o \
string/memcmp.o \
string/memcpy.o \
string/memmove.o \
string/memset.o \
string/stpcpy.o \
string/stpncpy.o \
string/strcat.o \
string/strcat_s.o \
string/strchr.o \
string/strcmp.o \
string/strcoll.o \
string/strcpy.o \
string/strcpy_s.o \
string/strcspn.o \
string/strlen.o \
string/strncat.o \
string/strncmp.o \
string/strncpy.o \
string/strnlen.o \
string/strpbrk.o \
string/strrchr.o \
string/strspn.o \
string/strstr.o \
string/strtok.o \
string/strxfrm.o \

BINARIES=libc.a libg.a libm.a $(CRT_OBJS)

all: $(BINARIES)

.PHONY: all clean install install-headers

libc.a: $(OBJS)
	$(AR) rcs $@ $(OBJS)
	strip -g $@

libg.a:
	$(AR) rcs $@ $(OBJS)

libm.a:
	$(AR) rcs $@

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
	rm -f src/syscall.inc $(BINARIES) $(OBJS)

install: $(BINARIES)
	mkdir -p $(DESTDIR)$(LIBDIR)
	cp $(BINARIES) $(DESTDIR)$(LIBDIR)

install-headers:
	mkdir -p $(DESTDIR)$(INCLUDEDIR)
	cp -RT include $(DESTDIR)$(INCLUDEDIR)
