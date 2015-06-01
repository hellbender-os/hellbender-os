target remote localhost:1234
symbol-file kernel/hellbender.kernel
add-symbol-file coresrv/core.srv 0x7f801010
add-symbol-file ~/build-busybox/busybox_unstripped 0x801090
