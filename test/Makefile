HOST?=i686-binary
HOSTARCH:=$(shell ../target-triplet-to-arch.sh $(HOST))

CFLAGS?=-O0 -g -DDEBUG
CPPFLAGS?=
LDFLAGS?=
LIBS?=

DESTDIR?=
PREFIX?=/usr/local
EXEC_PREFIX?=$(PREFIX)
BOOTDIR?=$(EXEC_PREFIX)/boot
INCLUDEDIR?=$(PREFIX)/include
LIBDIR?=$(EXEC_PREFIX)/lib

CFLAGS:=$(CFLAGS) -Wall -Wextra -I../sysroot/usr/include
CPPFLAGS:=$(CPPFLAGS) -Iinclude
LDFLAGS:=$(LDFLAGS)
LIBS:=$(LIBS) -static

CFLAGS:=$(CFLAGS)
CPFLAGS:=$(CPPFLAGS)
LDFLAGS:=$(LDFLAGS)
LIBS:=$(LIBS)

TESTCASES:=\
test_heap \
test_mem \

TEST_HEAP_OBJS:=\
../libc/heap/heap_init.host.o \
../libc/heap/heap_grow.host.o \
../libc/heap/heap_free.host.o \
../libc/heap/heap_malloc.host.o \
../libc/heap/heap_realloc.host.o \
../libc/heap/heap_debug.host.o \
test_heap.o \

TEST_MEM_OBJS:=\
../kernel/arch/i386/mem.host.o \
test_mem.o \

all: install

.PHONY: all clean install

test_heap: $(TEST_HEAP_OBJS) $(LIB_DEPS)
	$(CC) -o $@ $(CFLAGS) $(TEST_HEAP_OBJS) $(LDFLAGS) $(LIBS)

test_mem: $(TEST_MEM_OBJS) $(LIB_DEPS)
	$(CC) -o $@ $(CFLAGS) $(TEST_MEM_OBJS) $(LDFLAGS) $(LIBS)

%.o: %.c
	$(CC) -c $< -o $@ -std=gnu11 $(CFLAGS) $(CPPFLAGS)

%.o: %.S
	$(CC) -c $< -o $@ $(CFLAGS) $(CPPFLAGS)

%.host.o: %.c
	$(CC) -c $< -o $@ -std=gnu11 $(CFLAGS) $(CPPFLAGS)

%.host.o: %.S
	$(CC) -c $< -o $@ $(CFLAGS) $(CPPFLAGS)

clean:
	rm -f $(TESTCASES) $(OBJS)
#	rm -f *.o */*.o */*/*.o *~ */*~ */*/*~

install: $(TESTCASES)
	./test_mem
#	./test_heap
# TODO: run test cases
