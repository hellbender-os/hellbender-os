. ${srcdir}/emulparams/elf_x86_64.sh
TEXT_START_ADDR=0x200000
MAXPAGESIZE=0x1000
COMMONPAGESIZE=0x1000
INITIAL_READWRITE_SECTIONS=". = ALIGN(${COMMONPAGESIZE});"
