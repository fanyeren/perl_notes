#!/usr/bin/env perl

use strict;
use warnings;

use File::Glob ':glob';
use Carp qw(croak);

use List::Util qw(first);

my @time_seg = localtime(time - 86400);

my $yesterday = "" . ($time_seg[5] + 1900) . (sprintf("%02d", ($time_seg[4] + 1))) . (sprintf("%02d", $time_seg[3]));
my $yesterday_str = "" . ($time_seg[5] + 1900) . '-' . (sprintf("%02d", ($time_seg[4] + 1))) . '-' . (sprintf("%02d", $time_seg[3])) . ' ' . (sprintf("%02d", ($time_seg[2]))) . ':' . (sprintf("%02d", ($time_seg[1]))) . ':' . (sprintf("%02d", ($time_seg[1])));

my $imbs_log;
my $imbs_log_wf;

my $ns = 0;

if (-e "/home/work/nsimbs" && -l "/home/work/nsimbs") {
    $ns = 1;
    $imbs_log = "/home/work/nsimbs/log/imbs.log." . $yesterday . "*";
    $imbs_log_wf = "/home/work/nsimbs/log/imbs.log.wf." . $yesterday;
} else {
    $imbs_log = "/home/work/imbs/log/imbs.log." . $yesterday . "*";
    $imbs_log_wf = "/home/work/imbs/log/imbs.log.wf." . $yesterday;
}

eval {
    `mkdir -p /home/work/opbin/noah_stat/output && chmod 755 /home/work/opbin/noah_stat/output`
};

exit 1 if $@;

open my $predictor_output, ">", "/home/work/opbin/noah_stat/output/predictor.$yesterday" or croak "$!\n";

open my $predictor_wf, ">", "/home/work/opbin/noah_stat/predictor.wf" or croak "$!\n";
open my $predictor, ">", "/home/work/opbin/noah_stat/predictor" or croak "$!\n";

my @imbs_logs = bsd_glob($imbs_log);

open my $imbs_log_fd, "<", $imbs_log_wf or croak "$!\n";
while (my $line = <$imbs_log_fd>) {
    chomp $line;
    if ($line =~ m/predictor_api\ talk\ to\ server\ error|received\ error\ pack\ from\ predictor,\ err_code\(2004\)/x) {
        print $predictor_wf $line . "\n";
     }
}
print $predictor_wf "\n";


for my $file (@imbs_logs) {
    open my $fd, "<", $file or croak "$!\n";
    while (my $line = <$fd>) {
        chomp $line;

        my $qs_kpi = '';
        if ($line =~ m/qs=([^\ ]+)\ /x) {
            $qs_kpi = $1;
        }

        print $predictor "qs_kpi:" . $qs_kpi, "\n";
    }
}

print $predictor "\n";


my %ovlexp = ();

close $predictor_wf;
close $predictor;

open $predictor_wf, "<", "/home/work/opbin/noah_stat/predictor.wf" or croak "$!\n";
open $predictor, "<", "/home/work/opbin/noah_stat/predictor" or croak "$!\n";

my %exp_total = ();
my %exp_failed_total = ();
my $havest_loss = 0;
my $imbs_predictor = 0;

my %exp_total_coarse = ();
my %exp_failed_total_coarse = ();

map {
    my $exp = $_;
    $exp_total{$exp} = 0;
    $exp_failed_total{$exp} = 0;
} values %ovlexp;

map {
    my $exp = $_;
    $exp_total{$exp} = 0;
    $exp_failed_total{$exp} = 0;
} qw(QS IDEAQ WIDEAQ);

map {
    my $exp = $_;
    $exp_total_coarse{$exp} = 0;
    $exp_failed_total_coarse{$exp} = 0;
} qw(QS IDEAQ WIDEAQ);

while (my $line = <$predictor>) {
    chomp $line;
    if ($line =~ m/qs_kpi:\(\d+\|\d+\)\(\d+\|\d+\)\(\d+\|\d+\|\d+\)\((\d+)\)/) {
        my $count = $1;
        my $exp = 'QS';
        $exp_total{$exp} = (exists $exp_total{$exp} ? $exp_total{$exp} : 0) + $count;
        $imbs_predictor += $count;
    }
}

while (my $line = <$predictor_wf>) {
    chomp $line;
    $havest_loss += 1 if $line =~ m/received\ error\ pack\ from\ predictor,\ err_code\(2004\)/xms;
    if ($ns) {
        if ($line =~ m/predictor_api\ talk\ to\ server\ error/xms) {
            my $exp = "QS";
            $exp_failed_total{$exp} = (exists $exp_failed_total{$exp} ? $exp_failed_total{$exp} : 0) + 1;
        }
    } else {
        if ($line =~ m/predictor_api\ talk\ to\ server\ error.*service_name\(([0-9a-zA-Z\_]+)_group(\d+)\)/xms) {
            my $exp = $1;
            $exp =~ s/n//xms;
            $exp =~ s/_//xms;
            $exp = uc $exp;
            $exp_failed_total{$exp} = (exists $exp_failed_total{$exp} ? $exp_failed_total{$exp} : 0) + 1;
        }
    }
}


print $predictor_output $yesterday_str . "\t";

map {
    my $exp = $_;
    print $predictor_output "IMBS_$exp" . ":" . $exp_total{$exp}, "\t";
} keys %exp_total;

map {
    my $exp = $_;
    print $predictor_output "IMBS_$exp" . "_FAILED:" . $exp_failed_total{$exp}, "\t";
} keys %exp_failed_total;

print $predictor_output "IMBS_PREDICTOR" . ":" . $imbs_predictor, "\t";
print $predictor_output "IMBS_HAVEST" . "_LOSS:" . $havest_loss, "\t";

print $predictor_output "\n";

close $predictor_output;
