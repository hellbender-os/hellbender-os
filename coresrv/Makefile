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

OBJS:=\
$(ARCH_OBJS) \
src/core_main.o \

all: core.srv

.PHONY: all clean install install-headers install-server

core.srv: $(OBJS) $(ARCHDIR)/linker.ld $(LIB_DEPS)
	$(CC) -T $(ARCHDIR)/linker.ld -o $@ $(CFLAGS) $(OBJS) $(LDFLAGS) $(LIBS)

#$(DESTDIR)$(LIBDIR)/crti.o
#$(DESTDIR)$(LIBDIR)/crtn.o 

%.o: %.c
	$(CC) -c $< -o $@ -std=gnu11 $(CFLAGS) $(CPPFLAGS)

%.o: %.S
	$(CC) -c $< -o $@ $(CFLAGS) $(CPPFLAGS)

clean:
	rm -f core.srv $(OBJS)
#	rm -f *.o */*.o */*/*.o *~ */*~ */*/*~

install: install-headers install-server

install-headers:
	mkdir -p $(DESTDIR)$(INCLUDEDIR)
	cp -RT include $(DESTDIR)$(INCLUDEDIR)

install-server: core.srv
	mkdir -p $(DESTDIR)$(BOOTDIR)
	cp core.srv $(DESTDIR)$(BOOTDIR)
