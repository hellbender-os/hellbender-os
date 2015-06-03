        .data

        // These must be based at THREADLOCAL address in the threadlocal.h
        .global errno
        .global optarg
        .global opterr
        .global optind
        .global optopt

        .set errno,     0x402000
        .set optarg,    0x402004
        .set opterr,    0x402008
        .set optind,    0x40200c
        .set optopt,    0x402010
        
