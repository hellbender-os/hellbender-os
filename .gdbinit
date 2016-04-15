set arch i386:x86-64:intel
target remote localhost:1234
symbol-file kernel/hellbender.kernel.sym
break boot_64
continue
#disconnect
#set arch i386:x86-64
#target remote localhost:1234
