#!/bin/sh
set -e
. ./iso.sh

qemu-system-$(./target-triplet-to-arch.sh $HOST) -nographic -s -S -k fi -curses -cdrom hellbender.iso
