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

CORE_IDC:=coresrv/core_idc.o

CORE_OBJS:=\
$(HEADER_OBJ) \
$(ARCH_OBJS) \
$(ARCH_COREOBJS) \
coresrv/core_main.o \
kbd/kbd_init.o \
kbd/kbd_isr.o \
kbd/kbd_getc.o \
rtc/rtc_init.o \
rtc/rtc_isr.o \
rtc/rtc_ticks.o \
vga/vga_init.o \
vga/vga_putc.o \

APP_OBJS:=\
$(HEADER_OBJ) \
$(ARCH_OBJS) \
test/test_main.o \
coresrv/libcore.o \

all: core.srv test.app

.PHONY: all clean install install-headers install-server

core.srv: $(CORE_OBJS) $(CORE_IDC) $(ARCHDIR)/linker.ld $(LIB_DEPS)
	$(CC) -Xlinker -Map -Xlinker coresrv.map -T $(ARCHDIR)/linker.ld -o $@ $(CFLAGS) $(CORE_OBJS) $(CORE_IDC) $(LDFLAGS) $(LIBS)

coresrv/core_idc.S: $(CORE_OBJS) make_core_idc.sh
	./make_core_idc.sh > coresrv/core_idc.S

coresrv/libcore.S: coresrv.map make_libcore.pl
	./make_libcore.pl > coresrv/libcore.S

test.app: $(APP_OBJS) $(ARCHDIR)/linker.ld $(LIB_DEPS)
	$(CC) -T $(ARCHDIR)/linker.ld -o $@ $(CFLAGS) $(APP_OBJS) $(LDFLAGS) $(LIBS)
# To see the linker script: -Wl,-verbose
# To see the link process:  -Xlinker --print-map

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
