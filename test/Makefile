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
test_heap

LIBC_OBJS:=\
../libc/heap/heap_init.o \
../libc/heap/heap_grow.o \
../libc/heap/heap_free.o \
../libc/heap/heap_malloc.o \
../libc/heap/heap_realloc.o \
../libc/heap/heap_debug.o \

HOST_OBJS:=$(LIBC_OBJS:.o=.host.o)

OBJS:=\
$(HOST_OBJS) \
test_heap.o \


all: install

.PHONY: all clean install

test_heap: $(OBJS) $(LIB_DEPS)
	$(CC) -o $@ $(CFLAGS) $(OBJS) $(LDFLAGS) $(LIBS)

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
	./test_heap
# TODO: run test cases
