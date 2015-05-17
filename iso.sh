#!/bin/sh
set -e
. ./build.sh

mkdir -p isodir
mkdir -p isodir/boot
mkdir -p isodir/boot/grub

cp sysroot/boot/hellbender.kernel isodir/boot/hellbender.kernel
cp sysroot/boot/core.srv isodir/boot/core.srv
cp sysroot/boot/test.app isodir/boot/test.app
(cd initrd && find . | sort | cpio -o > ../isodir/boot/init.rd )
#objcopy --strip-debug isodir/boot/hellbender.kernel
cat > isodir/boot/grub/grub.cfg << EOF
menuentry "hellbender" {
    multiboot /boot/hellbender.kernel
    module /boot/core.srv --core
    module /boot/init.rd --initrd
    module /boot/test.app --test
}
EOF
grub-mkrescue -o hellbender.iso isodir
