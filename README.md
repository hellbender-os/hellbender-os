# hellbender-os
Hellbender Operating System - research project
----------------------------------------------

###  basic features:
  * micro-kernel approach.
  * multithreading.
  * memory protection.
  * interrupt processing in userspace.
  * 3 memory protection "domains": kernel, trusted userspace, untrusted userspace.

### kernel:
  * virtual memory allocation (fixed size blocks).
  * physical page allocation (linked list of free page -stacks).
  * memory mapping.
  * context switching.
  * scheduling (extremely simplistic).
  * interrupt-to-userspace routing.

### IPC:
  * POSIX semaphores (partial support).
  * special "non-context-switching-callee-memory-protecting" inter-domain call from untrusted userspace into trusted userspace.
  * trusted userspace callee has access to caller memory, avoiding message passing overhead with OS services.

### extremely limit standard library support:
  * malloc & friends.
  * printf.
  * abort.
  * exit.
  
### virtual address space split into three domains:
  * kernelspace in the first 4MB region.
  * OS services (trusted userspace code) at the top of the address space.
  * normal application userspace above 4MB, below services.
