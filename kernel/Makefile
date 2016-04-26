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

CFLAGS:=$(CFLAGS) -std=gnu11 -ffreestanding -fbuiltin -mcmodel=large -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -Wall -Wextra -Werror -fno-exceptions -fno-unwind-tables -fno-asynchronous-unwind-tables
#-s
CPPFLAGS:=$(CPPFLAGS) -D__is_hellbender_kernel -Iinclude
LDFLAGS:=$(LDFLAGS) -ffreestanding -nostdlib -z max-page-size=0x1000 -Wl,--build-id=none
#-Wl,--strip-debug -Wl,-s
LIBS:=$(LIBS) -static -nostdlib -lgcc -lk

OBJS:=\
src/multiboot.o \
src/boot_32.o \
src/boot_64.o \
src/gdt.o \
src/cpu.o \
src/cpu_asm.o \
src/kernel.o \
src/process.o \
src/thread.o \
src/lomem.o \
src/page.o \
src/himem.o \
src/heap.o \
src/isr.o \
src/isr_asm.o \
src/isr_routine.o \
src/idt.o \
src/pic.o \
src/syscall_resolve.o \
src/syscall.o \
src/signal.o \
src/scheduler.o \

all: hellbender

.PHONY: all clean install install-headers

hellbender: $(OBJS) linker.ld
	$(CC) -Wl,-Map,hellbender.map -T linker.ld -o $@.elf64 $(LDFLAGS) $(OBJS) $(LIBS)
	objcopy --strip-debug -I elf64-x86-64 -O elf32-i386 $@.elf64 $@
	objcopy --only-keep-debug $@.elf64 $@.sym

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS) $(CPPFLAGS)

%.o: %.S src/struct2asm.inc src/syscall_resolve.inc
	$(CC) -c $< -o $@ $(CFLAGS) -DASM

src/struct2asm.inc: src/struct2asm.c
	$(CC) -S $< -o - | grep '^->' | awk '($$1 == "->") { print "#define " $$2 " " substr($$3,2) }' > $@

src/syscall_resolve.inc: src/syscall.c
	echo "// Automagically generated my Makefile, do not edit." > $@
	grep " syscall_" $< | perl -ne '/^uint64_t\s+([^(]+)/; print ".quad $$1\n";' >> $@

include/hellbender/syscall.h: src/syscall.c src/syscall.h.in
	echo "// Automagically generated my Makefile, do not edit." > $@
	cat src/syscall.h.in >> $@
	grep SYSCALL_INT src/config.h >> $@
	grep " syscall_" src/syscall.c | tr "{" ";" >> $@
	echo "#endif" >> $@

clean:
	rm $(OBJS) src/struct2asm.inc src/syscall_resolve.inc include/hellbender/syscall.h hellbender.map hellbender.elf64 hellbender

install: hellbender
	mkdir -p $(DESTDIR)$(BOOTDIR)
	cp hellbender $(DESTDIR)$(BOOTDIR)

install-headers: include/hellbender/syscall.h
	mkdir -p $(DESTDIR)$(INCLUDEDIR)
	cp -RT include $(DESTDIR)$(INCLUDEDIR)
