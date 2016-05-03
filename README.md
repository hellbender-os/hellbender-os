# hellbender-os
Hellbender Operating System - research project
==============================================

This is a 64-bit rewrite based on the same principles as the
original 32-bit version:

  * micro-kernel approach.
  * multithreading.
  * memory protection.
  * interrupt processing in userspace.
  * 3 memory protection "domains": kernel, trusted userspace, untrusted userspace.
  * some standard C library + posix library support.

See [wiki](https://github.com/hellbender-os/hellbender-os/wiki)
for more information.

