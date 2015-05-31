HOST?=$(shell ../default-host.sh)
HOSTARCH:=$(shell ../target-triplet-to-arch.sh $(HOST))

CFLAGS?=
CPPFLAGS?=
LDFLAGS?=
LIBS?=

DESTDIR?=
PREFIX?=/usr/local
EXEC_PREFIX?=$(PREFIX)
BOOTDIR?=$(EXEC_PREFIX)/boot
INCLUDEDIR?=$(PREFIX)/include
LIBDIR?=$(EXEC_PREFIX)/lib

CFLAGS:=$(CFLAGS) -ffreestanding -fbuiltin -Wall -Wextra -Werror
#-s -fno-exceptions -fno-unwind-tables -fno-asynchronous-unwind-tables
CPPFLAGS:=$(CPPFLAGS) -D__is_hellbender_kernel -Iinclude
LDFLAGS:=$(LDFLAGS)
#-Wl,--strip-debug -Wl,-s
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
syscall/syscall_kstub.o \
syscall/syscall_table.o \
syscall/get_environment.o \
syscall/current_exec_path.o \
syscall/spawn.o \
syscall/exit.o \
syscall/make_executable.o \
syscall/make_readonly.o \
syscall/set_program_break.o \
syscall/print.o \
syscall/yield.o \
syscall/wait_eq.o \
syscall/wait_neq.o \
syscall/notify.o \

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

HEADERS=$(shell find . -name "*\.h")

all: hellbender.kernel

.PHONY: all clean install install-headers install-kernel

hellbender.kernel: $(ALL_OUR_OBJS) $(ARCHDIR)/linker.ld $(LIB_DEPS)
	$(CC) -Wl,-Map,kernel.map -T $(ARCHDIR)/linker.ld -o $@ $(CFLAGS) $(OBJ_LINK_LIST) $(LDFLAGS) $(LIBS)
#	strip -R .comment $@

syscall/syscall_stub.S: $(HEADERS) make_syscall_size.pl make_syscall_stub.pl
	./make_syscall_size.pl
	$(CC) -S syscall/syscall_size.c -o syscall/syscall_size.S -std=gnu11 $(CFLAGS) $(CPPFLAGS)
	./make_syscall_stub.pl

%.o: %.c
	$(CC) -c $< -o $@ -std=gnu11 $(CFLAGS) $(CPPFLAGS)

%.o: %.S
	$(CC) -c $< -o $@ $(CFLAGS) $(CPPFLAGS)

clean:
	rm -f hellbender.kernel $(ALL_OUR_OBJS)
#	rm -f *.o */*.o */*/*.o *~ */*~ */*/*~

install: install-headers install-kernel

install-headers: syscall/syscall_stub.o
	mkdir -p $(DESTDIR)$(INCLUDEDIR)
	cp -RT include $(DESTDIR)$(INCLUDEDIR)

install-kernel: hellbender.kernel
	mkdir -p $(DESTDIR)$(BOOTDIR)
	cp hellbender.kernel $(DESTDIR)$(BOOTDIR)
