#!/usr/bin/env perl

my %total_packet = ();
$total_packet{'ASQ'} = 0;
$total_packet{'WASQ'} = 0;

# qspn=3 ws=0

while (<STDIN>) {
    chmop;
    my $line = $_;
    if ($line =~ m/qspn=(\d+)\s/xms) {
        my $pack_num = $1;
        if ($line =~ m/ws=0/xms) {
            $total_packet{'ASQ'} = $total_packet{'ASQ'} + $pack_num;
        } elsif ($line =~ m/ws=1/xms) {
            $total_packet{'WASQ'} = $total_packet{'WASQ'} + $pack_num;
        }
    }
}

print "ASQ: ", $total_packet{'ASQ'}, "\n";
print "WASQ: ", $total_packet{'WASQ'}, "\n";
