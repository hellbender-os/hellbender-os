HOST?=$(shell ../default-host.sh)
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

CFLAGS:=$(CFLAGS) -s -ffreestanding -fbuiltin -fno-exceptions -fno-unwind-tables -fno-asynchronous-unwind-tables -Wall -Wextra
CPPFLAGS:=$(CPPFLAGS) -D__is_hellbender_kernel -Iinclude
LDFLAGS:=$(LDFLAGS) -Wl,--strip-debug -Wl,-s
LIBS:=$(LIBS) -static -nostdlib -lk -lgcc
LIB_DEPS:=$(DESTDIR)/$(LIBDIR)/libk.a
ARCHDIR:=arch/$(HOSTARCH)

include $(ARCHDIR)/make.config

CFLAGS:=$(CFLAGS) $(KERNEL_ARCH_CFLAGS)
CPFLAGS:=$(CPPFLAGS) $(KERNEL_ARCH_CPPFLAGS)
LDFLAGS:=$(LDFLAGS) $(KERNEL_ARCH_LDFLAGS)
LIBS:=$(LIBS) $(KERNEL_ARCH_LIBS)

OBJS:=\
$(KERNEL_ARCH_OBJS) \
early/stage_1.o \
early/stage_2.o \
early/stage_3.o \
early/stage_4.o \
kernel/kernel.o \
kernel/isr_debugger.o \
kernel/trap_isr.o \
kernel/pic_isr.o \
kernel/kernel_isr.o \
kernel/domain.o \
kernel/module.o \
kernel/thread.o \
kernel/tss.o \
kernel/scheduler.o \
kernel/pit.o \
kernel/pic.o \
kernel/process.o \
kernel/semaphore.o \

CRTI_OBJ:=$(ARCHDIR)/crti.o
CRTBEGIN_OBJ:=$(shell $(CC) $(CFLAGS) $(LDFLAGS) -print-file-name=crtbegin.o)
CRTEND_OBJ:=$(shell $(CC) $(CFLAGS) $(LDFLAGS) -print-file-name=crtend.o)
CRTN_OBJ:=$(ARCHDIR)/crtn.o

ALL_OUR_OBJS:=\
$(CRTI_OBJ) \
$(OBJS) \
$(CRTN_OBJ) \

OBJ_LINK_LIST:=\
$(CRTI_OBJ) \
$(CRTBEGIN_OBJ) \
$(OBJS) \
$(CRTEND_OBJ) \
$(CRTN_OBJ) \

all: hellbender.kernel

.PHONY: all clean install install-headers install-kernel

hellbender.kernel: $(ALL_OUR_OBJS) $(ARCHDIR)/linker.ld $(LIB_DEPS)
	$(CC) -Wl,-Map,kernel.map -T $(ARCHDIR)/linker.ld -o $@ $(CFLAGS) $(OBJ_LINK_LIST) $(LDFLAGS) $(LIBS)
	strip -R .comment $@

%.o: %.c
	$(CC) -c $< -o $@ -std=gnu11 $(CFLAGS) $(CPPFLAGS)

%.o: %.S
	$(CC) -c $< -o $@ $(CFLAGS) $(CPPFLAGS)

clean:
	rm -f hellbender.kernel $(ALL_OUR_OBJS)
#	rm -f *.o */*.o */*/*.o *~ */*~ */*/*~

install: install-headers install-kernel

install-headers:
	mkdir -p $(DESTDIR)$(INCLUDEDIR)
	cp -RT include $(DESTDIR)$(INCLUDEDIR)

install-kernel: hellbender.kernel
	mkdir -p $(DESTDIR)$(BOOTDIR)
	cp hellbender.kernel $(DESTDIR)$(BOOTDIR)
