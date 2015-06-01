#!/bin/sh
set -e
. ./build.sh

mkdir -p isodir
mkdir -p isodir/boot
mkdir -p isodir/boot/grub

cp sysroot/boot/hellbender.kernel isodir/boot/hellbender.kernel
cp sysroot/boot/core.srv isodir/boot/core.srv
cp utils/bin/init initrd/bin/init
cp utils/bin/hellish initrd/bin/hellish
cp /home/hellbender/build-busybox/_install/bin/busybox initrd/bin/busybox

(cd initrd && { find . -type d -printf "%p/\n"; find . \! -type d ; } | sort | cpio -v -o > ../isodir/boot/init.rd )
objcopy --strip-debug isodir/boot/hellbender.kernel
objcopy --strip-debug isodir/boot/core.srv
cat > isodir/boot/grub/grub.cfg << EOF
menuentry "hellbender" {
    multiboot /boot/hellbender.kernel
    module /boot/core.srv --core
    module /boot/init.rd --initrd
}
EOF
grub-mkrescue -o hellbender.iso isodir
