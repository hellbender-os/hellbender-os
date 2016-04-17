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
src/multiboot_asm.o \
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
src/list.o \
src/himem.o \
src/heap.o \
src/isr.o \
src/isr_asm.o \
src/idt.o \
src/pic.o \

all: hellbender.kernel

.PHONY: all clean install install-headers

hellbender.kernel: $(OBJS) linker.ld
	$(CC) -Wl,-Map,hellbender.kernel.map -T linker.ld -o $@.elf64 $(LDFLAGS) $(OBJS) $(LIBS)
	objcopy --strip-debug -I elf64-x86-64 -O elf32-i386 $@.elf64 $@
	objcopy --only-keep-debug $@.elf64 $@.sym

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS) $(CPPFLAGS)

%.o: %.S src/struct2asm.h
	$(CC) -c $< -o $@ $(CFLAGS)

src/struct2asm.h: src/struct2asm.c
	$(CC) -S $< -o - | grep '^->' | awk '($$1 == "->") { print "#define " $$2 " " substr($$3,2) }' > $@

clean:
	rm $(OBJS) hellbender.kernel.map hellbender.kernel.elf64 hellbender.kernel

install: hellbender.kernel
	mkdir -p $(DESTDIR)$(BOOTDIR)
	cp hellbender.kernel $(DESTDIR)$(BOOTDIR)

install-headers:
	mkdir -p $(DESTDIR)$(INCLUDEDIR)
	cp -RT include $(DESTDIR)$(INCLUDEDIR)
