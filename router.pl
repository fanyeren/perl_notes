#!/usr/bin/env perl

use strict;
use warnings;

use File::Glob ':glob';

use List::Util qw(first);

my @time_seg = localtime(time - 86400);

my $yesterday = "" . ($time_seg[5] + 1900) . (sprintf("%02d", ($time_seg[4] + 1))) . (sprintf("%02d", $time_seg[3]));
my $yesterday_str = "" . ($time_seg[5] + 1900) . '-' . (sprintf("%02d", ($time_seg[4] + 1)))
    . '-' . (sprintf("%02d", $time_seg[3])) . ' ' . (sprintf("%02d", ($time_seg[2]))) . ':'
    . (sprintf("%02d", ($time_seg[1]))) . ':' . (sprintf("%02d", ($time_seg[1])));

my $router_log = "/home/work/router/log/router.log." . $yesterday . "*";

open my $router_output, ">", "/home/work/opbin/noah_stat/output/router.$yesterday" or die "$!\n";

open my $router, ">", "/home/work/opbin/noah_stat/router" or die "$!\n";

my @router_logs = bsd_glob($router_log);

for my $file (@router_logs) {
    open my $fd, "<", $file or die "$!\n";
    while (my $line = <$fd>) {
        chomp $line;

        my $err = '0';
        if ($line =~ m/err=([^\ ]+)\ /x) {
            $err = $1;
        }

        my $ovlexp = '';
        if ($line =~ m/rpf=([^\ ]+)\ /x) {
            $ovlexp = $1;
        }

        print $router "err:" . $err . " " . "ovlexp:" . $ovlexp, "\n";
    }
}


my %ovlexp = ();

$ovlexp{'PE0'}='E0AS';
$ovlexp{'PE1'}='E1AS';
$ovlexp{'PE2'}='E2AS';
$ovlexp{'PE3'}='E3AS';
$ovlexp{'PE4'}='E4AS';
$ovlexp{'PE5'}='E5AS';
$ovlexp{'PE6'}='E6AS';
$ovlexp{'PE7'}='E7AS';
$ovlexp{'PE9'}='E9AS';
$ovlexp{'PD0'}='D0AS';
$ovlexp{'PD1'}='D1AS';
$ovlexp{'PD2'}='D2AS';
$ovlexp{'PD3'}='D3AS';
$ovlexp{'PD4'}='D4AS';
$ovlexp{'PD5'}='D5AS';
$ovlexp{'PD6'}='D6AS';
$ovlexp{'PD7'}='D7AS';
$ovlexp{'PD8'}='D8AS';
$ovlexp{'PD9'}='D9AS';

close $router;

open $router, "<", "/home/work/opbin/noah_stat/router" or die "$!\n";

my %exp_total = ();
my %exp_failed_total = ();

map {
    my $exp = $_;
    $exp_total{$exp} = 0;
    $exp_failed_total{$exp} = 0;
} values %ovlexp;

while (my $line = <$router>) {
    chomp $line;

    my $err;
    my $ovlexp;
    my @segs;

    if ($line =~ m/err:(\d+)\ ovlexp:(\S+)/xms) {
        $err = $1;
        $ovlexp = $2;

        next unless exists $ovlexp{$ovlexp};
        my $exp = $ovlexp{$ovlexp};
        $exp_total{$exp} = $exp_total{$exp} + 1;

        $exp_failed_total{$exp} = $exp_failed_total{$exp} + 1 if ($err > 0);
    }
}

print $router_output $yesterday_str . "\t";

map {
    my $exp = $_;
    print $router_output "ROUTER_$exp" . ":" . $exp_total{$exp}, "\t"; } keys %exp_total;

map {
    my $exp = $_;
    print $router_output "ROUTER_$exp" . "_FAILED:" . $exp_failed_total{$exp}, "\t"; } keys %exp_failed_total;

print $router_output "\n";

close $router_output;
