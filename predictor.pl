#!/usr/bin/env perl

use strict;
use warnings;

use File::Glob ':glob';

use List::Util qw(first);

my @time_seg = localtime(time - 86400);

my $yesterday = "" . ($time_seg[5] + 1900) . (sprintf("%02d", ($time_seg[4] + 1))) . (sprintf("%02d", $time_seg[3]));
my $yesterday_str = "" . ($time_seg[5] + 1900) . '-' . (sprintf("%02d", ($time_seg[4] + 1))) . '-' . (sprintf("%02d", $time_seg[3])) . ' ' . (sprintf("%02d", ($time_seg[2]))) . ':' . (sprintf("%02d", ($time_seg[1]))) . ':' . (sprintf("%02d", ($time_seg[1])));

my $imas_log = "/home/work/imas/log/imas.log." . $yesterday . "*";
my $imas_log_wf = "/home/work/imas/log/imas.log.wf." . $yesterday;

open my $predictor_output, ">", "/home/work/opbin/noah_stat/output/predictor.$yesterday" or die "$!\n";

open my $predictor_wf, ">", "/home/work/opbin/noah_stat/predictor.wf" or die "$!\n";
open my $predictor_pc, ">", "/home/work/opbin/noah_stat/predictor.pc" or die "$!\n";
open my $predictor_wise, ">", "/home/work/opbin/noah_stat/predictor.wise" or die "$!\n";

my @imas_logs = bsd_glob($imas_log);

open my $imas_log_fd, "<", $imas_log_wf or die "$!\n";
while (my $line = <$imas_log_fd>) {
    chomp $line;
    if ($line =~ m/predictor_api\ talk\ to\ server\ error/x) {
        print $predictor_wf $line . "\n";
     }
}
print $predictor_wf "\n";


for my $file (@imas_logs) {
    open my $fd, "<", $file or die "$!\n";
    while (my $line = <$fd>) {
        chomp $line;
        my $wise = 0;
        if ($line =~ m/ws=1/x) {
            $wise = 1
        }

        my $qs_kpi = '';
        if ($line =~ m/qs_kpi=([^\ ]+)\ /x) {
            $qs_kpi = $1;
        }

        my $ovlexp = '';
        if ($line =~ m/ovlexp=([^\ ]+)\ /x) {
            $ovlexp = $1;
        }
        if ($wise) {
            print $predictor_wise "qs_kpi:" . $qs_kpi . " " . "ovlexp:" . $ovlexp, "\n";
        } else {
            print $predictor_pc "qs_kpi:" . $qs_kpi . " " . "ovlexp:" . $ovlexp, "\n";
        }
    }
}

print $predictor_wise "\n";
print $predictor_pc "\n";


my %ovlexp = ();

#$ovlexp{'120-0'} = 'D0AS';
#$ovlexp{'206L-10001'}='D1AS';
#$ovlexp{'206L-10002'}='D2AS';
#$ovlexp{'120-3'}='D3AS';
#$ovlexp{'120-4'}='D4AS';
#$ovlexp{'120-5'}='D5AS';
#$ovlexp{'206L-10003'}='D6AS';
#$ovlexp{'120-7'}='D7AS';
#$ovlexp{'120-8'}='D8AS';
#$ovlexp{'120-9'}='D9AS';
#$ovlexp{'121-0'}='D0BS';
#$ovlexp{'121-1'}='D1BS';
#$ovlexp{'121-2'}='D2BS';
#$ovlexp{'121-3'}='D3BS';
#$ovlexp{'122-0'}='D0CS';
#$ovlexp{'122-1'}='D1CS';
#$ovlexp{'122-2'}='D2CS';
#$ovlexp{'122-3'}='D3CS';
#$ovlexp{'122-4'}='D4CS';
#$ovlexp{'122-5'}='D5CS';
$ovlexp{'207L-1010'}='D1CLKQS';
$ovlexp{'207L-1011'}='D0CLKQS';
$ovlexp{'387-0'}='D0ASQ';
$ovlexp{'387-1'}='D1ASQ';
$ovlexp{'207L-2'}='D2ASQ';
$ovlexp{'207L-3'}='D3ASQ';
$ovlexp{'207L-4'}='D4ASQ';
$ovlexp{'207L-5'}='D5ASQ';
$ovlexp{'207L-6'}='D6ASQ';
$ovlexp{'207L-7'}='D7ASQ';
$ovlexp{'207L-8'}='D8ASQ';
$ovlexp{'207L-10'}='D10ASQ';
$ovlexp{'247L-0'}='D0WASQ';
$ovlexp{'247L-1'}='D1WASQ';
$ovlexp{'247L-2'}='D2WASQ';
$ovlexp{'247L-3'}='D3WASQ';
$ovlexp{'247L-4'}='D4WASQ';
$ovlexp{'247L-5'}='D5WASQ';
$ovlexp{'247L-6'}='D6WASQ';
$ovlexp{'527-7'}='D7WASQ';

close $predictor_wf;
close $predictor_wise;
close $predictor_pc;

open $predictor_wf, "<", "/home/work/opbin/noah_stat/predictor.wf" or die "$!\n";
open $predictor_pc, "<", "/home/work/opbin/noah_stat/predictor.pc" or die "$!\n";
open $predictor_wise, "<", "/home/work/opbin/noah_stat/predictor.wise" or die "$!\n";

my %exp_total = ();
my %exp_failed_total = ();

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
} qw(WASQ ASQ CLKQS);

map {
    my $exp = $_;
    $exp_total_coarse{$exp} = 0;
    $exp_failed_total_coarse{$exp} = 0;
} qw(WASQ ASQ CLKQS);

while (my $line = <$predictor_wise>) {
    chomp $line;

    my $qs_kpi;
    my $ovlexp;
    my @segs;

    my @exps = ();

    if ($line =~ m/qs_kpi:(\S+)\ ovlexp:(.*)$/xms) {
        $qs_kpi = $1;
        $ovlexp = $2;

        @segs = split(/#/, $ovlexp);
        if (@segs && scalar(@segs) > 0) {
            for my $seg (@segs) {
                if (exists $ovlexp{$seg}) {
                    push @exps, $ovlexp{$seg};
                }
            }
        }

        $qs_kpi =~ s/\(|\)/ /g;

        @segs = split(/ /, $qs_kpi);
        if (@segs && scalar(@segs) > 0) {
            for my $seg (@segs) {
                next if $seg eq '';
                next if $seg =~ m/ /;

                if ($seg =~ m/(\d+)\|(\d+)\|(\d+)/xms) {
                    my $x = $1;
                    my $y = $2;
                    my $z = $3;

                    #wasq
                    my $exp;

                    if ($x == '0' || $x == '1' || $x == '3' || $x == '4' || $x == '6') {
                        if ($ovlexp eq '') {
                            $exp = 'WASQ';
                            $exp_total{$exp} = $exp_total{$exp} + $y;
                            #$exp_failed_total{$exp} = $exp_failed_total{$exp} + $z;
                        } else {
                            $exp = first { $_ =~ m/WASQ$/x } @exps;
                            if (defined $exp) {
                                $exp_total{$exp} = $exp_total{$exp} + $y;
                            } else {
                                $exp = 'WASQ';
                                $exp_total{$exp} = $exp_total{$exp} + $y;
                            }
                        }
                    } elsif ($x == '2') {
                        if ($ovlexp eq '') {
                            $exp = 'CLKQS';
                            $exp_total{$exp} = $exp_total{$exp} + $y;
                        } else {
                            $exp = first { $_ =~ m/CLKQS$/x } @exps;
                            if (defined $exp) {
                                $exp_total{$exp} = $exp_total{$exp} + $y;
                            } else {
                                $exp = 'CLKQS';
                                $exp_total{$exp} = $exp_total{$exp} + $y;
                            }
                        }
                    }
                }
            }
        }
    }
}


while (my $line = <$predictor_pc>) {
    chomp $line;

    my $qs_kpi;
    my $ovlexp;
    my @segs;

    my @exps = ();

    if ($line =~ m/qs_kpi:(\S+)\ ovlexp:(.*)$/xms) {
        $qs_kpi = $1;
        $ovlexp = $2;

        @segs = split(/#/, $ovlexp);
        if (@segs && scalar(@segs) > 0) {
            for my $seg (@segs) {
                if (exists $ovlexp{$seg}) {
                    push @exps, $ovlexp{$seg};
                }
            }
        }

        $qs_kpi =~ s/\(|\)/ /g;

        @segs = split(/ /, $qs_kpi);
        if (@segs && scalar(@segs) > 0) {
            for my $seg (@segs) {
                next if $seg eq '';
                next if $seg =~ m/ /;

                if ($seg =~ m/(\d+)\|(\d+)\|(\d+)/xms) {
                    my $x = $1;
                    my $y = $2;
                    my $z = $3;

                    #asq
                    my $exp;

                    if ($x == '0') {
                        if ($ovlexp eq '') {
                            $exp = 'ASQ';
                            $exp_total{$exp} = $exp_total{$exp} + $y;
                        } else {
                            $exp = first { $_ =~ m/ASQ$/x } @exps;
                            if (defined $exp) {
                                $exp_total{$exp} = $exp_total{$exp} + $y;
                            } else {
                                $exp = 'ASQ';
                                $exp_total{$exp} = $exp_total{$exp} + $y;
                            }
                        }
                    } elsif ($x == '1') {
                        if ($ovlexp eq '') {
                            $exp = 'CLKQS';
                            $exp_total{$exp} = $exp_total{$exp} + $y;
                        } else {
                            $exp = first { $_ =~ m/CLKQS$/x } @exps;
                            if (defined $exp) {
                                $exp_total{$exp} = $exp_total{$exp} + $y;
                                #$exp_failed_total{$exp} = $exp_failed_total{$exp} + $z;
                            } else {
                                $exp = 'CLKQS';
                                $exp_total{$exp} = $exp_total{$exp} + $y;
                            }
                        }
                    }
                }
            }
        }
    }
}

while (my $line = <$predictor_wf>) {
    chomp $line;
    if ($line =~ m/predictor_api\ talk\ to\ server\ error.*service_name\(([0-9a-zA-Z\_]+)_group0\)/xms) {
        my $exp = $1;
        $exp =~ s/n//xms;
        $exp =~ s/_//xms;
        $exp = uc $exp;
        $exp_failed_total{$exp} = (exists $exp_failed_total{$exp} ? $exp_failed_total{$exp} : 0) + 1;
    }
}


print $predictor_output $yesterday_str . "\t";

map {
    my $exp = $_;
    print $predictor_output "IMAS_$exp" . ":" . $exp_total{$exp}, "\t";
} keys %exp_total;

map {
    my $exp = $_;
    print $predictor_output "IMAS_$exp" . "_FAILED:" . $exp_failed_total{$exp}, "\t";
} keys %exp_failed_total;

print $predictor_output "\n";

close $predictor_output;
