#!/bin/sh
set -e
. ./build.sh

mkdir -p isodir/boot/grub
cp -r sysroot/boot/* isodir/boot/

cat > isodir/boot/grub/grub.cfg << EOF
menuentry "hellbender" {
    multiboot /boot/hellbender.kernel
}
EOF
grub-mkrescue -o hellbender.iso isodir
