HOST?=i686-binary
HOSTARCH:=$(shell ../target-triplet-to-arch.sh $(HOST))

CFLAGS?=-O2 -g0
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
LIB_DEPS:=$(DESTDIR)/$(LIBDIR)/libc.a
ARCHDIR:=arch/$(HOSTARCH)

include $(ARCHDIR)/make.config

CFLAGS:=$(CFLAGS) $(ARCH_CFLAGS)
CPFLAGS:=$(CPPFLAGS) $(ARCH_CPPFLAGS)
LDFLAGS:=$(LDFLAGS) $(ARCH_LDFLAGS) -nostartfiles
LIBS:=$(LIBS) $(ARCH_LIBS)

HEADER_OBJ:=$(ARCHDIR)/module_header.o

CORE_IDC:=coresrv/coresrv_idc.o
CORE_IDCH:=include/coresrv/coresrv_idc.h
CORE_STUB:=coresrv/coresrv_stub.o
CORE_SYMS:=coresrv/coresrv_syms.o

CORE_OBJS:=\
$(HEADER_OBJ) \
$(ARCH_OBJS) \
$(ARCH_COREOBJS) \
coresrv/core_main.o \
ps2/ps2_init.o \
ps2/ps2_isr.o \
kbd/kbd_init.o \
kbd/kbd_isr.o \
kbd/kbd_getc.o \
rtc/rtc_init.o \
rtc/rtc_isr.o \
rtc/rtc_ticks.o \
rtc/rtc_beep.o \
vga/vga_init.o \
vga/vga_putc.o \
vfs/vfs_init.o \
vfs/vfs_mount.o \
vfs/vfs_open.o \
vfs/rootfs.o \
vfs/initfs.o \
vfs/devfs.o \
dev/dev_init.o \
dev/dev_register.o \
dev/dev_unregister.o \
dev/tty.o \
elf/load_process.o \

CRTI_OBJ:=$(DESTDIR)/$(LIBDIR)/crti.o
CRTBEGIN_OBJ:=$(shell $(CC) $(CFLAGS) $(LDFLAGS) -print-file-name=crtbegin.o)
CRTEND_OBJ:=$(shell $(CC) $(CFLAGS) $(LDFLAGS) -print-file-name=crtend.o)
CRTN_OBJ:=$(DESTDIR)/$(LIBDIR)/crtn.o

ALL_OUR_OBJS:=\
$(CORE_OBJS) \
$(CORE_IDC) \
$(CORE_STUB) \
$(CORE_SYMS) \
$(HEADER_OB) \

OBJ_LINK_LIST:=\
$(CRTI_OBJ) \
$(CRTBEGIN_OBJ) \
$(CORE_OBJS) \
$(CORE_IDC) \
$(CORE_STUB) \
$(CRTEND_OBJ) \
$(CRTN_OBJ) \

HEADERS=$(shell find . -name "*\.h")

all: core.srv

.PHONY: all clean install install-headers install-server

core.srv: $(ALL_OUR_OBJS) $(ARCHDIR)/linker.ld $(LIB_DEPS)
	$(CC) -Wl,-Map,coresrv.map -T $(ARCHDIR)/linker.ld -o $@ $(CFLAGS) $(OBJ_LINK_LIST) $(LDFLAGS) $(LIBS)

coresrv/coresrv_idc.S include/coresrv/coresrv_idc.h: $(HEADERS) make_coresrv_idc.sh
	./make_coresrv_idc.sh > coresrv/coresrv_idc.S 2> $(CORE_IDCH)

coresrv/coresrv_stub.S: $(CORE_OBJS) make_coresrv_stub.sh
	./make_coresrv_stub.sh > coresrv/coresrv_stub.S

coresrv/coresrv_syms.S: coresrv.map make_coresrv_syms.pl
	./make_coresrv_syms.pl > coresrv/coresrv_syms.S

%.o: %.c $(HEADERS)
	$(CC) -c $< -o $@ -std=gnu11 $(CFLAGS) $(CPPFLAGS)

%.o: %.S
	$(CC) -c $< -o $@ $(CFLAGS) $(CPPFLAGS)

clean:
	rm -f core.srv $(ALL_OUR_OBJS)
#	rm -f *.o */*.o */*/*.o *~ */*~ */*/*~

install: install-headers install-libraries install-server

install-headers: $(CORE_IDCH)
	mkdir -p $(DESTDIR)$(INCLUDEDIR)
	cp -RT include $(DESTDIR)$(INCLUDEDIR)

install-libraries: $(CORE_STUB) $(CORE_SYMS)
	mkdir -p $(DESTDIR)$(LIBDIR)
	cp $(CORE_STUB) $(CORE_SYMS) $(DESTDIR)$(LIBDIR)

install-server: core.srv
	mkdir -p $(DESTDIR)$(BOOTDIR)
	cp core.srv $(DESTDIR)$(BOOTDIR)
