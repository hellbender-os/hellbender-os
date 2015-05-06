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
LDFLAGS:=$(LDFLAGS) -nostartfiles
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

HEADER_OBJ:=$(ARCHDIR)/module_header.o

CRTI_OBJ:=$(DESTDIR)/$(LIBDIR)/crt0.o $(DESTDIR)/$(LIBDIR)/crti.o
CRTBEGIN_OBJ:=$(shell $(CC) $(CFLAGS) $(LDFLAGS) -print-file-name=crtbegin.o)
CRTEND_OBJ:=$(shell $(CC) $(CFLAGS) $(LDFLAGS) -print-file-name=crtend.o)
CRTN_OBJ:=$(DESTDIR)/$(LIBDIR)/crtn.o

ALL_OUR_OBJS:=\
$(HEADER_OBJ) \
$(OBJS) \

OBJ_LINK_LIST:=\
$(HEADER_OBJ) \
$(CRTI_OBJ) \
$(CRTBEGIN_OBJ) \
$(OBJS) \
$(CRTEND_OBJ) \
$(CRTN_OBJ) \

all: core.srv

.PHONY: all clean install install-headers install-server

core.srv: $(OBJ_LINK_LIST) $(ARCHDIR)/linker.ld $(LIB_DEPS)
	$(CC) -T $(ARCHDIR)/linker.ld -o $@ $(CFLAGS) $(OBJ_LINK_LIST) $(LDFLAGS) $(LIBS)

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
