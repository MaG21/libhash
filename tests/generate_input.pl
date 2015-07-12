#!/usr/bin/perl
# $Id: generate_input.pl,v 1.2 2005/02/06 17:00:10 andrew Exp $

# Help look for memory leaks

$reps = (defined($ARGV[0]) ? $ARGV[0] : 125000);
$| = 1;

print STDERR "Adding entries...\n";

for ($i = 1; $i <= $reps; ++$i) {
	print($i, " ", 'abc' x (($i % 10) + 1), "\n");
}

print STDERR "Entries added...\n";

print("0\n0\n0\n");

print STDERR "Removing entries...\n";

for ($i = 1; $i <= $reps; ++$i) {
	print(-$i, "\n");
}

print STDERR "Entries removed...\n";
