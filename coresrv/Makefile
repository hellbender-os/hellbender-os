HOST?=i686-binary
HOSTARCH:=$(shell ../target-triplet-to-arch.sh $(HOST))

CFLAGS?=-O2 -g0 -dDEBUG
CPPFLAGS?=
LDFLAGS?=
LIBS?=

DESTDIR?=
PREFIX?=/usr/local
EXEC_PREFIX?=$(PREFIX)
BOOTDIR?=$(EXEC_PREFIX)/boot
INCLUDEDIR?=$(PREFIX)/include
LIBDIR?=$(EXEC_PREFIX)/lib

CFLAGS:=$(CFLAGS) -Wall -Wextra
CPPFLAGS:=$(CPPFLAGS) -Iinclude
LDFLAGS:=$(LDFLAGS)
LIBS:=$(LIBS) -static
LIB_DEPS:=$(DESTDIR)/usr/lib/libc.a
ARCHDIR:=arch/$(HOSTARCH)

include $(ARCHDIR)/make.config

CFLAGS:=$(CFLAGS) $(ARCH_CFLAGS)
CPFLAGS:=$(CPPFLAGS) $(ARCH_CPPFLAGS)
LDFLAGS:=$(LDFLAGS) $(ARCH_LDFLAGS)
LIBS:=$(LIBS) $(ARCH_LIBS)

HEADER_OBJ:=$(ARCHDIR)/module_header.o

CORE_OBJS:=\
$(HEADER_OBJ) \
$(ARCH_OBJS) \
$(ARCH_COREOBJS) \
src/core_main.o \
src/keyboard.o \
src/timer.o \

APP_OBJS:=\
$(HEADER_OBJ) \
$(ARCH_OBJS) \
src/test_main.o \

all: core.srv test.app

.PHONY: all clean install install-headers install-server

core.srv: $(CORE_OBJS) $(ARCHDIR)/linker.ld $(LIB_DEPS)
	$(CC) -T $(ARCHDIR)/linker.ld -o $@ $(CFLAGS) $(CORE_OBJS) $(LDFLAGS) $(LIBS)

test.app: $(APP_OBJS) $(ARCHDIR)/linker.ld $(LIB_DEPS)
	$(CC) -T $(ARCHDIR)/linker.ld -o $@ $(CFLAGS) $(APP_OBJS) $(LDFLAGS) $(LIBS)

%.o: %.c
	$(CC) -c $< -o $@ -std=gnu11 $(CFLAGS) $(CPPFLAGS)

%.o: %.S
	$(CC) -c $< -o $@ $(CFLAGS) $(CPPFLAGS)

clean:
	rm -f core.srv $(OBJS)
	rm -f test.app $(APP_OBJS)
#	rm -f *.o */*.o */*/*.o *~ */*~ */*/*~

install: install-headers install-server

install-headers:
	mkdir -p $(DESTDIR)$(INCLUDEDIR)
	cp -RT include $(DESTDIR)$(INCLUDEDIR)

install-server: core.srv test.app
	mkdir -p $(DESTDIR)$(BOOTDIR)
	cp core.srv $(DESTDIR)$(BOOTDIR)
	cp test.app $(DESTDIR)$(BOOTDIR)
