#!/usr/bin/env perl

use strict;
use warnings;

use Carp qw(croak);
use File::Glob ':glob';
my $predictor_log = "/home/work/xiahoufeng/melete/logflow/output/*";
my @predictor_logs = bsd_glob($predictor_log);

my $ASQ_FAILED = 0;
my %ASQ_FAILED = ();
my %SRC_ASQ_FAILED = ();
my $WASQ_FAILED = 0;
my %WASQ_FAILED = ();
my %SRC_WASQ_FAILED = ();
my $QS_FAILED = 0;
my %QS_FAILED = ();
my %SRC_QS_FAILED = ();

my $src = '';

for my $file (@predictor_logs) {
    open my $fd, "<", $file or croak "$!\n";
    if ($file =~ m/([^\/]+)_predictor\.wf/xms) {
        $file = $1;
    }
    while (<$fd>) {
        chomp;
        my $line = $_;

        if ($line =~ m/server_ip\((\d+\.\d+\.\d+\.\d+)\).*n_asq_group\d+/) {
            $ASQ_FAILED{$1} = (exists $ASQ_FAILED{$1}) ? $ASQ_FAILED{$1} + 1 : 1;
            $ASQ_FAILED += 1;
            $SRC_ASQ_FAILED{$file} = (exists $SRC_ASQ_FAILED{$file}) ? $SRC_ASQ_FAILED{$file} + 1 : 1;
        }

        elsif ($line =~ m/server_ip\((\d+\.\d+\.\d+\.\d+)\).*n_wasq_group\d+/) {
            $WASQ_FAILED{$1} = (exists $WASQ_FAILED{$1}) ? $WASQ_FAILED{$1} + 1 : 1;
            $WASQ_FAILED += 1;
            $SRC_WASQ_FAILED{$file} = (exists $SRC_WASQ_FAILED{$file}) ? $SRC_WASQ_FAILED{$file} + 1 : 1;
        }

        elsif ($line =~ m/server_ip\((\d+\.\d+\.\d+\.\d+)\).*n_qs_group\d+/) {
            $QS_FAILED{$1} = (exists $QS_FAILED{$1}) ? $QS_FAILED{$1} + 1 : 1;
            $QS_FAILED += 1;
            $SRC_QS_FAILED{$file} = (exists $SRC_QS_FAILED{$file}) ? $SRC_QS_FAILED{$file} + 1 : 1;
        }
    }
}


print "\n\nPS/PG/DATA IMAS -> ASQ FAILED TOTAL:\t" . $ASQ_FAILED . "\n";

my $top_ip_inter = 0;
my $top_ip_count = 20;

print "\nimas\n";
$top_ip_inter = 0;

for my $ip(sort { $SRC_ASQ_FAILED{$b} <=> $SRC_ASQ_FAILED{$a} } keys %SRC_ASQ_FAILED) {
    last if $top_ip_inter >= $top_ip_count;
    print "$ip:\t\t$SRC_ASQ_FAILED{$ip}\n";
    $top_ip_inter += 1;
}

print "\nasq\n";
$top_ip_inter = 0;
for my $ip(sort { $ASQ_FAILED{$b} <=> $ASQ_FAILED{$a} } keys %ASQ_FAILED) {
    last if $top_ip_inter >= $top_ip_count;
    print "$ip:\t\t\t$ASQ_FAILED{$ip}\n";
    $top_ip_inter += 1;
}


print "\n\nPS/PG/DATA IMAS -> WASQ FAILED TOTAL:\t" . $WASQ_FAILED . "\n";

print "\nimas\n";
$top_ip_inter = 0;
for my $ip(sort { $SRC_WASQ_FAILED{$b} <=> $SRC_WASQ_FAILED{$a} } keys %SRC_WASQ_FAILED) {
    last if $top_ip_inter >= $top_ip_count;
    print "$ip:\t\t$SRC_WASQ_FAILED{$ip}\n";
    $top_ip_inter += 1;
}
print "\nwasq\n";
$top_ip_inter = 0;
for my $ip(sort { $WASQ_FAILED{$b} <=> $WASQ_FAILED{$a} } keys %WASQ_FAILED) {
    last if $top_ip_inter >= $top_ip_count;
    print "$ip:\t\t\t$WASQ_FAILED{$ip}\n";
    $top_ip_inter += 1;
}

print "\n\nPS/PG/DATA BS -> QS FAILED TOTAL:\t" . $QS_FAILED . "\n";
print "\nimbs\n";
$top_ip_inter = 0;

for my $ip(sort { $SRC_QS_FAILED{$b} <=> $SRC_QS_FAILED{$a} } keys %SRC_QS_FAILED) {
    last if $top_ip_inter >= $top_ip_count;
    print "$ip:\t\t$SRC_QS_FAILED{$ip}\n";
    $top_ip_inter += 1;
}
print "\nbsq\n";
$top_ip_inter = 0;
for my $ip(sort { $QS_FAILED{$b} <=> $QS_FAILED{$a} } keys %QS_FAILED) {
    last if $top_ip_inter >= $top_ip_count;
    print "$ip:\t\t\t$QS_FAILED{$ip}\n";
    $top_ip_inter += 1;
}
