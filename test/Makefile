HOST?=i686-binary
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

CFLAGS:=-std=gnu11 -fno-builtin -Dvfprintf=my_vfprintf -O0 -g -DDEBUG -Wall -Wextra -I../sysroot/usr/include
CPPFLAGS:=$(CPPFLAGS) -Iinclude
LDFLAGS:=$(LDFLAGS)
LIBS:=$(LIBS) -static

CFLAGS:=$(CFLAGS)
CPFLAGS:=$(CPPFLAGS)
LDFLAGS:=$(LDFLAGS)
LIBS:=$(LIBS)

TESTCASES:=\
test_heap \
test_printf \

TEST_HEAP_OBJS:=\
test_heap.o \
../libc/heap/heap_init.host.o \
../libc/heap/heap_grow.host.o \
../libc/heap/heap_free.host.o \
../libc/heap/heap_malloc.host.o \
../libc/heap/heap_realloc.host.o \
../libc/heap/heap_debug.host.o \

TEST_PRINTF_OBJS:=\
test_printf.o \
../libc/stdio/fprintf.host.o \
../libc/stdio/sprintf.host.o \
../libc/stdio/fputc.host.o \
../libc/stdio/fflush.host.o \
../libc/stdlib/lltoa.host.o \
../libc/stdlib/ulltoa.host.o \

OBJS:=\
$(TEST_HEAP_OBJS) \
$(TEST_PRINTF_OBJS) \

all: install

.PHONY: all clean install

test_heap: $(TEST_HEAP_OBJS) $(LIB_DEPS)
	$(CC) -o bin/$@ $(CFLAGS) $(TEST_HEAP_OBJS) $(LDFLAGS) $(LIBS)

test_printf: $(TEST_PRINTF_OBJS) $(LIB_DEPS)
	$(CC) -o bin/$@ $(CFLAGS) $(TEST_PRINTF_OBJS) $(LDFLAGS) $(LIBS)

%.o: %.c
	$(CC) -c $< -o $@ -std=gnu11 $(CFLAGS) $(CPPFLAGS)

%.o: %.S
	$(CC) -c $< -o $@ $(CFLAGS) $(CPPFLAGS)

%.host.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS) $(CPPFLAGS)

%.host.o: %.S
	$(CC) -c $< -o $@ $(CFLAGS) $(CPPFLAGS)

clean:
	rm -f $(TESTCASES) $(OBJS)
#	rm -f *.o */*.o */*/*.o *~ */*~ */*/*~

install: $(TESTCASES)
	./bin/test_printf
#	./bin/test_heap
# TODO: run test cases
