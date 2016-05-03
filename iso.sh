#!/bin/sh
set -e
. ./build.sh

mkdir -p isodir/boot/grub
cp -r sysroot/boot/* isodir/boot/
(cd initrd && { find . -type d -printf "%p/\n"; find . \! -type d ; } | sort | cpio -v -o > ../isodir/boot/initrd )

cat > isodir/boot/grub/grub.cfg << EOF
menuentry "hellbender" {
    multiboot /boot/hellbender
    module /boot/coresrv --coresrv
    module /boot/initrd --initrd
}
EOF
grub-mkrescue -o hellbender.iso isodir
