#! /usr/bin/perl -w
    eval 'exec /usr/bin/perl -S $0 ${1+"$@"}'
        if 0; #$running_under_some_shell

use strict;
use File::Find ();

# Set the variable $File::Find::dont_use_nlink if you're using AFS,
# since AFS cheats.

# for the convenience of &wanted calls, including -eval statements:
use vars qw/*name *dir *prune/;
*name   = *File::Find::name;
*dir    = *File::Find::dir;
*prune  = *File::Find::prune;

sub wanted;

open(SIZES, ">syscall/syscall_size.c") or die "$!";
print SIZES "// Automatically generated by make_syscall_size.pl\n";
print SIZES "#include <kernel/syscall.h>\n";

my $next_idx = 0;

# Traverse desired filesystems
File::Find::find({wanted => \&wanted}, 'include');

print SIZES "\nvoid main() {}\n";
close(SIZES);
exit;

sub wanted {
    /^.*\.h\z/s;
    #&& print("$name\n");
    open(HEADER, "<$_") or die "$!";
    while (<HEADER>) {
        if (/__SYS__\s+(__attribute__\(\(.*?\)\)\s*)?([^\(]+)\(([^\)]*)\)/) {
            my $func = $2;
            my $params = $3;
            $func =~ /(.*?)\s+(\S+)$/;
            my $retval = $1;
            my $name = $2;
            my $idx = $next_idx++;
            #print("idx=$idx; func '$name'; returns '$retval'; params '$params'\n");
            print SIZES "\n";
            print SIZES "struct __attribute__((__packed__)) {\n";
            my @fields = split(",", $params);
            foreach my $field (@fields) {
                $field =~ s/^\s*//;
                $field =~ s/\s*$//;
                print SIZES "  $field;\n";
            }
            print SIZES "} _${name}__size;\n";
        }
    }
    close(HEADER);
}
