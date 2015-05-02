#!/bin/sh
set -e
. ./iso.sh

qemu-system-$(./target-triplet-to-arch.sh $HOST) -curses -cdrom hellbender.iso
