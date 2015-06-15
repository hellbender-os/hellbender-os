        .section .absolute
        
        // thread has the following structure:
        // BASE:
        //  kernel data page
        //  kernel stack
        //  user info page (read only)
        //  user data page (r/w)
        //  thread heap
        //  thread stack

        .global _thread_base
        .set _thread_base, 0x400000

        .global _thread_kdata
        .set _thread_kdata, _thread_base
        
        .global _thread_kstack
        .set _thread_kstack, _thread_kdata + 0x1000
        
        .global _thread_kstack_top
        .set _thread_kstack_top, _thread_kstack + 0x1000

        .global _thread_uinfo
        .set _thread_uinfo, _thread_kstack_top

        .global _thread_udata
        .set _thread_udata, _thread_uinfo + 0x1000
        
        .global _thread_heap
        .set _thread_heap, _thread_udata + 0x1000
        
        .global _thread_heap_top
        .set _thread_heap_top, _thread_heap + 0x100000

        .global _thread_stack_top
        .set _thread_stack_top, _thread_base + 0x400000
        
        .global _thread_stack
        .set _thread_stack, _thread_stack_top - 0x100000
        
      
        // These must be as in libc/hellbender/threadlocal.h

        .global errno
        .global optarg
        .global opterr
        .global optind
        .global optopt
  
        .set errno,     _thread_udata + 0x000
        .set optarg,    _thread_udata + 0x004
        .set opterr,    _thread_udata + 0x008
        .set optind,    _thread_udata + 0x00c
        .set optopt,    _thread_udata + 0x010
