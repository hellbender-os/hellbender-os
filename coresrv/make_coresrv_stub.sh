#!/bin/bash

echo "// this file was automatically generated by make_coresrv_stub.sh"
echo ".section .text"
echo ""
echo ".global coresrv_stub"
echo "coresrv_stub:"
echo "    pop %edx"
echo "    mov (%esp), %eax"
echo "    int \$82;"
echo ""
find . -name "*\.h" -exec grep -E "^\s*__IDC__" {} \; | perl -ne '/^\s*__IDC__\s+\S+\s+([^\(]+)/; print "$1\n";' | sort | uniq | perl -ne '/(.*)/; print "\n.global __IDC__$1\n.set __IDC__$1, coresrv_stub\n";' 
