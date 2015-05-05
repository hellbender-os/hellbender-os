#!/bin/sh
set -e
. ./build.sh

mkdir -p isodir
mkdir -p isodir/boot
mkdir -p isodir/boot/grub

cp sysroot/boot/hellbender.kernel isodir/boot/hellbender.kernel
cp sysroot/boot/core.srv isodir/boot/core.srv
objcopy --strip-debug isodir/boot/hellbender.kernel
cat > isodir/boot/grub/grub.cfg << EOF
menuentry "hellbender" {
    multiboot /boot/hellbender.kernel
    module /boot/core.srv --core
}
EOF
grub-mkrescue -o hellbender.iso isodir
