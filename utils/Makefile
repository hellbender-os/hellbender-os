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

CFLAGS:=$(CFLAGS)  -s -fno-exceptions -fno-unwind-tables -fno-asynchronous-unwind-tables -Wall -Wextra -Werror
CPPFLAGS:=$(CPPFLAGS) -Iinclude
LDFLAGS:=$(LDFLAGS)
LIBS:=$(LIBS) -static
LIB_DEPS:=$(DESTDIR)/usr/lib/libc.a
ARCHDIR:=arch/$(HOSTARCH)

INIT_OBJS:=\
init/init_main.o \

HELLISH_OBJS:=\
hellish/hellish_main.o \

all: init hellish

.PHONY: all clean install

init: $(INIT_OBJS) $(LIB_DEPS)
	$(CC) -Wl,-Map,init.map -o bin/$@ $(CFLAGS) $(INIT_OBJS) $(LDFLAGS) $(LIBS)

hellish: $(HELLISH_OBJS) $(LIB_DEPS)
	$(CC) -Wl,-Map,hellish.map -o bin/$@ $(CFLAGS) $(HELLISH_OBJS) $(LDFLAGS) $(LIBS)

%.o: %.c $(HEADERS)
	$(CC) -c $< -o $@ -std=gnu11 $(CFLAGS) $(CPPFLAGS)

clean:
	rm -f bin/* $(INIT_OBJS) $(HELLISH_OBJS)
#	rm -f *.o */*.o */*/*.o *~ */*~ */*/*~

install: init hellish
