HOST?=$(shell ../default-host.sh)
HOSTARCH:=$(shell ../target-triplet-to-arch.sh $(HOST))

CFLAGS?=
CPPFLAGS?=
LDFLAGS?=
LIBS?=

DESTDIR?=$(SYSROOT)
PREFIX?=/usr/local
EXEC_PREFIX?=$(PREFIX)
BOOTDIR?=$(EXEC_PREFIX)/boot
INCLUDEDIR?=$(PREFIX)/include
LIBDIR?=$(EXEC_PREFIX)/lib

CFLAGS:=$(CFLAGS) -fPIC -std=gnu11 -mcmodel=large -mno-mmx -mno-sse -mno-sse2 -Wall -Wextra -Werror -fno-exceptions -fno-unwind-tables -fno-asynchronous-unwind-tables
#-s
CPPFLAGS:=$(CPPFLAGS) -D__is_hellbender_coresrv -Iinclude
LDFLAGS:=$(LDFLAGS) -z max-page-size=0x1000 -Wl,--build-id=none
#-Wl,--verbose
#-Wl,--strip-debug -Wl,-s
LIBS:=$(LIBS) -static

OBJS:=\
src/main.o \
src/broker.o \
fs/vfs.o \
fs/initfs.o \
fs/tmpfs.o \
fs/devfs.o \
fs/fat16.o \
dev/console.o \
dev/ramdisk.o \
dev/pci.o \
dev/ps2.o \

all: coresrv

.PHONY: all clean install install-headers

coresrv: $(OBJS)
	$(CC) -Wl,-Map,coresrv.map -o $@.elf64 $(CFLAGS) $(LDFLAGS) $(OBJS) $(LIBS)
	objcopy --strip-debug -I elf64-x86-64 -O elf32-i386 $@.elf64 $@
	objcopy --only-keep-debug $@.elf64 $@.sym

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS) $(CPPFLAGS)

%.o: %.S
	$(CC) -c $< -o $@ $(CFLAGS) -DASM

clean:
	rm -f $(OBJS) coresrv.map coresrv.elf64 coresrv

install: coresrv
	mkdir -p $(DESTDIR)$(BOOTDIR)
	cp coresrv $(DESTDIR)$(BOOTDIR)

install-headers:
	mkdir -p $(DESTDIR)$(INCLUDEDIR)
	cp -RT include $(DESTDIR)$(INCLUDEDIR)
