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

CFLAGS:=$(CFLAGS) -std=gnu11 -ffreestanding -fbuiltin -mcmodel=large -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -Wall -Wextra -Werror -fno-exceptions -fno-unwind-tables -fno-asynchronous-unwind-tables
# -s
CPPFLAGS:=$(CPPFLAGS) -D__is_hellbender_libk -Iinclude

OBJS:=\
build/stdlib/abs.o \
build/stdlib/atoi.o \
build/stdlib/bsearch.o \
build/stdlib/div.o \
build/stdlib/itoa.o \
build/stdlib/labs.o \
build/stdlib/ldiv.o \
build/stdlib/lltoa.o \
build/stdlib/qsort.o \
build/stdlib/strtol.o \
build/stdlib/ulltoa.o \
build/stdlib/utoa.o \
build/string/memccpy.o \
build/string/memchr.o \
build/string/memcmp.o \
build/string/memcpy.o \
build/string/memmove.o \
build/string/memset.o \
build/string/stpcpy.o \
build/string/stpncpy.o \
build/string/strcat.o \
build/string/strcat_s.o \
build/string/strchr.o \
build/string/strcmp.o \
build/string/strcoll.o \
build/string/strcpy.o \
build/string/strcpy_s.o \
build/string/strcspn.o \
build/string/strlen.o \
build/string/strncat.o \
build/string/strncmp.o \
build/string/strncpy.o \
build/string/strnlen.o \
build/string/strpbrk.o \
build/string/strrchr.o \
build/string/strspn.o \
build/string/strstr.o \
build/string/strtok.o \
build/string/strxfrm.o \

all: libk.a

.PHONY: all clean install install-headers

libk.a: $(OBJS)
	$(AR) rcs $@ $(OBJS)

build/%.o: ../libc/%.c
	$(CC) -c $< -o $@ $(CFLAGS) $(CPPFLAGS)

build/%.o: ../libc/%.S
	$(CC) -c $< -o $@ $(CFLAGS) $(CPPFLAGS)

clean:
	rm -f libk.a $(OBJS)

install: libk.a
	mkdir -p $(DESTDIR)$(LIBDIR)
	cp libk.a $(DESTDIR)$(LIBDIR)

install-headers:
