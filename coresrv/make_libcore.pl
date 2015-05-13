#!/usr/bin/perl

print "        // This file was automatically generated from coresrv.map\n";
print "        .section .text\n";

open(my $map, "<", "coresrv.map")
    or die "cannot poen coresrv.map";
while (<$map>) {
    if (/^\s+(0x\S+)\s+idc_(\S+)/) {
        print "\n";
        print "        .global $2\n";
        print "$2:\n";
        print "        mov \$$1, %eax\n";
        print "        int \$82\n";
    }
}
