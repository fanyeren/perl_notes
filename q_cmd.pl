#!/usr/bin/env perl

my %hash = ();

while (my $line = <STDIN>) {
    chomp $line;
    if ($line =~ /cmd\(([a-z0-9\/]+)\).*service_name\((.*)_group[\d+]\)/xms) {
        $hash{"$2_$1"} = (exists $hash{"$2_$1"} ? $hash{"$2_$1"} : 0) + 1
    }
}

map {
    print $hash{$_}, ": ", $_, "\n";
} keys %hash;
