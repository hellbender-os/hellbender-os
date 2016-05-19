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

OBJS:=\

S_OBJS := $(OBJS:.o=S.o)

BINARIES=libpthread.a libpthreadG.a libpthreadS.a libpthreadSG.a

all: $(BINARIES)

.PHONY: all clean install install-headers

libpthread.a: $(OBJS)
	$(AR) rcs $@ $(OBJS)
	strip -g $@

libpthreadG.a: $(OBJS)
	$(AR) rcs $@ $(OBJS)

libpthreadS.a: $(S_OBJS)
	$(AR) rcs $@ $(S_OBJS)
	strip -g $@

libpthreadSG.a: $(S_OBJS)
	$(AR) rcs $@ $(S_OBJS)

%S.o: %.c
	$(CC) -c $< -o $@ -fPIC $(CFLAGS) $(CPPFLAGS)

%S.o: %.S
	$(CC) -c $< -o $@ -fPIC $(CFLAGS) $(CPPFLAGS) -DASM

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS) $(CPPFLAGS)

%.o: %.S
	$(CC) -c $< -o $@ $(CFLAGS) $(CPPFLAGS) -DASM

clean:
	rm -f $(BINARIES) $(OBJS)

install: $(BINARIES)
	mkdir -p $(DESTDIR)$(LIBDIR)
	cp $(BINARIES) $(DESTDIR)$(LIBDIR)

install-headers:
	mkdir -p $(DESTDIR)$(INCLUDEDIR)
	cp -RT include $(DESTDIR)$(INCLUDEDIR)
