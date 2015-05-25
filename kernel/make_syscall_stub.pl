#! /usr/bin/perl -w

open(SIZES, "<syscall/syscall_size.S") or die "$!";

open(TABLE, ">syscall/syscall_table.S") or die "$!";
print TABLE "// Automatically generated by make_syscall_stub.pl\n";
print TABLE ".section .rodata\n";
print TABLE ".global syscall_table\n";
print TABLE ".balign 4096\n";
print TABLE "syscall_table:\n";

open(STUBS, ">syscall/syscall_stub.S") or die "$!";
print STUBS "// Automatically generated by make_syscall_stub.pl\n";
print STUBS ".text\n";

open(STUBS2, ">syscall/syscall_kstub.S") or die "$!";
print STUBS2 "// Automatically generated by make_syscall_stub.pl\n";
print STUBS2 ".text\n";

my $next_idx = 0;

while (<SIZES>) {
    if (/\.comm\s+_([^,]+?)__size\s*,([^,]+)/) {
        my $name = $1;
        my $size = $2;
        my $idx = $next_idx++;
        print TABLE "\n";
        print TABLE ".extern $name\n";
        print TABLE ".int $name\n";
        print TABLE ".int $size\n";

        print STUBS "\n.global $name\n";
        print STUBS "$name:\n";
        print STUBS "  mov \$$idx, %eax\n";
        print STUBS "  int \$80\n";
        print STUBS "  ret\n";

        print STUBS2 "\n.global k$name\n";
        print STUBS2 "k$name:\n";
        print STUBS2 "  mov \$$idx, %eax\n";
        print STUBS2 "  int \$80\n";
        print STUBS2 "  ret\n";
    }
}

print TABLE "\n.balign 4096\n";

close(TABLE);
close(SIZES);
close(STUBS);
close(STUBS2);
