#!/bin/bash

echo "// this file was automatically generated by make_coresrv_idc.sh"
echo ".section .idc_table"
echo ".org 8 // first slot is reserved"
find . -name "*\.c" -exec grep -E "^\s*__IDCIMPL__" {} \; | perl -ne '/^\s*__IDCIMPL__\s+\S+\s+([^\(]+)/; print "$1\n";' | sort | uniq | perl -ne '/(.*)/; print "\n.global __IDE__$1\n__IDE__$1:\n    calll $1\n    int \$83\n.balign 8\n";'
echo ""
echo ".balignl 4096, 0x53cd5a90"

>&2 echo "// this file was automatically generated by make_coresrv_idc.sh"
find . -name "*\.h" -exec grep -E "^\s*__IDC__" {} \; | perl -ne '/^\s*__IDC__\s+(\S+)\s+([^\(]+)(\([^\)]+\))/; print "$1 $2 $3\n";' | sort | uniq | perl -ne '/(\S+) (\S+) (.+)/; print "\n$1 __IDC__$2$3;\ntypedef void* $2_ptr;\n";' 1>&2
>&2 echo ""
find . -name "*\.c" -exec grep -E "^\s*__IDCIMPL__" {} \; | perl -ne '/^\s*__IDCIMPL__\s+(\S+)\s+([^\(]+)(\([^\)]+\))/; print "$2\n";' | sort | uniq | perl -ne '/(.*)/; print "extern uintptr_t __IDE__$1;\n";' 1>&2
