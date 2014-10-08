#!/usr/bin/env perl

use strict;
use warnings;

use File::Glob ':glob';
use Carp qw(croak);

use List::Util qw(first);

my @time_seg = localtime(time - 86400);

my $yesterday = "" . ($time_seg[5] + 1900) . (sprintf("%02d", ($time_seg[4] + 1))) . (sprintf("%02d", $time_seg[3]));
my $yesterday_str = "" . ($time_seg[5] + 1900) . '-' . (sprintf("%02d", ($time_seg[4] + 1))) . '-' . (sprintf("%02d", $time_seg[3])) . ' ' . (sprintf("%02d", ($time_seg[2]))) . ':' . (sprintf("%02d", ($time_seg[1]))) . ':' . (sprintf("%02d", ($time_seg[1])));

my $imas_log;
my $imas_log_wf;

if (-e "/home/work/nsimas" && -l "/home/work/nsimas") {
    $imas_log = "/home/work/nsimas/log/imas.log." . $yesterday . "*";
    $imas_log_wf = "/home/work/nsimas/log/imas.log.wf." . $yesterday;
} else {
    $imas_log = "/home/work/imas/log/imas.log." . $yesterday . "*";
    $imas_log_wf = "/home/work/imas/log/imas.log.wf." . $yesterday;
}

eval {
    `mkdir -p /home/work/opbin/noah_stat/output && chmod 755 /home/work/opbin/noah_stat/output`
};

exit 1 if $@;

#-------------------------
open my $predictor_output, ">", "/home/work/opbin/noah_stat/output/predictor.$yesterday" or croak "$!\n";

open my $predictor_wf, ">", "/home/work/opbin/noah_stat/predictor.wf" or croak "$!\n";
open my $predictor_pc, ">", "/home/work/opbin/noah_stat/predictor.pc" or croak "$!\n";
open my $predictor_wise, ">", "/home/work/opbin/noah_stat/predictor.wise" or croak "$!\n";

#-------------------------
open my $imbs_output, ">", "/home/work/opbin/noah_stat/output/imbs.$yesterday" or croak "$!\n";

open my $imbs_wf, ">", "/home/work/opbin/noah_stat/imbs.wf" or croak "$!\n";
open my $imbs_pc, ">", "/home/work/opbin/noah_stat/imbs.pc" or croak "$!\n";
open my $imbs_wise, ">", "/home/work/opbin/noah_stat/imbs.wise" or croak "$!\n";

#-------------------------
open my $imcs_output, ">", "/home/work/opbin/noah_stat/output/imcs.$yesterday" or croak "$!\n";

open my $imcs_wf, ">", "/home/work/opbin/noah_stat/imcs.wf" or croak "$!\n";
open my $imcs_pc, ">", "/home/work/opbin/noah_stat/imcs.pc" or croak "$!\n";
open my $imcs_wise, ">", "/home/work/opbin/noah_stat/imcs.wise" or croak "$!\n";


open my $imdi_output, ">", "/home/work/opbin/noah_stat/output/imdi.$yesterday" or croak "$!\n";
open my $imdi_wf, ">", "/home/work/opbin/noah_stat/imdi.wf" or croak "$!\n";

# usergate
open my $upin_output, ">", "/home/work/opbin/noah_stat/output/upin.$yesterday" or croak "$!\n";
open my $upin_wf, ">", "/home/work/opbin/noah_stat/upin.wf" or croak "$!\n";

open my $vsq_output, ">", "/home/work/opbin/noah_stat/output/vsq.$yesterday" or croak "$!\n";
open my $vsq_wf, ">", "/home/work/opbin/noah_stat/vsq.wf" or croak "$!\n";

my $imas_flow = 0;
my @imas_logs = bsd_glob($imas_log);

open my $imas_log_fd, "<", $imas_log_wf or croak "$!\n";
while (my $line = <$imas_log_fd>) {
    chomp $line;
    if ($line =~ m/predictor_api\ talk\ to\ server\ error|received\ error\ pack\ from\ predictor,\ err_code\(2004\)/x) {
        print $predictor_wf $line . "\n";
    }
    elsif ($line =~ m/(?:bs\ group.*\ is\ not\ valid|all\ bs\ groups\ failed),\ platform/x) {
        print $imbs_wf $line . "\n";
    }
    elsif ($line =~ m/interact\ with\ ras[a-z0-9]+\ failed/x) {
        print $imcs_wf $line . "\n";
    }
    elsif ($line =~ m/request\ imdi\ fail,\ reason/x) {
        print $imdi_wf $line . "\n";
    }
    elsif ($line =~ m/talk to upin failed/x) {
        print $upin_wf $line . "\n";
    }
}

print $predictor_wf "\n";
print $imbs_wf "\n";
print $imcs_wf "\n";
print $imdi_wf "\n";
print $upin_wf "\n";

my $imas_predictor = 0;

for my $file (@imas_logs) {
    open my $fd, "<", $file or croak "$!\n";
    while (my $line = <$fd>) {
        chomp $line;
        my $wise = 0;
        if ($line =~ m/ws=1/x) {
            $wise = 1
        }

        if ($line =~ m/qspn=(\d+)/x) {
            $imas_predictor += $1;
        }

        my $qs_kpi = '';
        if ($line =~ m/qs_kpi=([^\ ]+)\ /x) {
            $qs_kpi = $1;
        }

        my $bsrn;
        if ($line =~ m/bsrn=(\([^\ ]+\))\ /x) {
            $bsrn = $1;
            $imas_flow += 1;
        }

        my $csfn;
        if ($line =~ m/csfn=(\([^\ ]+\))\ /x) {
            $csfn = $1;
        }

        my $ovlexp = '';
        if ($line =~ m/ovlexp=([^\ ]+)\ /x) {
            $ovlexp = $1;
        }

        if ($wise) {
            print $predictor_wise "qs_kpi:" . $qs_kpi . " " . "ovlexp:" . $ovlexp, "\n";
            print $imbs_wise "bsrn:" . $bsrn . " " . "ovlexp:" . $ovlexp, "\n" if defined $bsrn;
            print $imcs_wise "csfn:" . $csfn . " " . "ovlexp:" . $ovlexp, "\n" if defined $csfn;
        } else {
            print $predictor_pc "qs_kpi:" . $qs_kpi . " " . "ovlexp:" . $ovlexp, "\n";
            print $imbs_pc "bsrn:" . $bsrn . " " . "ovlexp:" . $ovlexp, "\n" if defined $bsrn;
            print $imcs_pc "csfn:" . $csfn . " " . "ovlexp:" . $ovlexp, "\n" if defined $csfn;
        }
    }
}

print $predictor_wise "\n";
print $predictor_pc "\n";

print $imbs_wise "\n";
print $imbs_pc "\n";

print $imcs_wise "\n";
print $imcs_pc "\n";


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
$ovlexp{'121-0'}='D0BS';
$ovlexp{'121-1'}='D1BS';
$ovlexp{'121-2'}='D2BS';
$ovlexp{'121-3'}='D3BS';
$ovlexp{'122-0'}='D0CS';
$ovlexp{'122-1'}='D1CS';
$ovlexp{'122-2'}='D2CS';
$ovlexp{'122-3'}='D3CS';
$ovlexp{'122-4'}='D4CS';
$ovlexp{'122-5'}='D5CS';
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
#$ovlexp{'207L-10'}='D10ASQ';
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

close $imbs_wf;
close $imbs_wise;
close $imbs_pc;

close $imcs_wf;
close $imcs_wise;
close $imcs_pc;

close $imdi_wf;
close $upin_wf;

#----------------------------------------------------------------

open $predictor_wf, "<", "/home/work/opbin/noah_stat/predictor.wf" or croak "$!\n";
open $predictor_pc, "<", "/home/work/opbin/noah_stat/predictor.pc" or croak "$!\n";
open $predictor_wise, "<", "/home/work/opbin/noah_stat/predictor.wise" or croak "$!\n";

open $imbs_wf, "<", "/home/work/opbin/noah_stat/imbs.wf" or croak "$!\n";
open $imbs_pc, "<", "/home/work/opbin/noah_stat/imbs.pc" or croak "$!\n";
open $imbs_wise, "<", "/home/work/opbin/noah_stat/imbs.wise" or croak "$!\n";

open $imcs_wf, "<", "/home/work/opbin/noah_stat/imcs.wf" or croak "$!\n";
open $imcs_pc, "<", "/home/work/opbin/noah_stat/imcs.pc" or croak "$!\n";
open $imcs_wise, "<", "/home/work/opbin/noah_stat/imcs.wise" or croak "$!\n";


open $imdi_wf, "<", "/home/work/opbin/noah_stat/imdi.wf" or croak "$!\n";
open $upin_wf, "<", "/home/work/opbin/noah_stat/upin.wf" or croak "$!\n";
open $vsq_wf, "<", "/home/work/opbin/noah_stat/vsq.wf" or croak "$!\n";

my %exp_total = ();
my %exp_failed_total = ();
my $havest_loss = 0;

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
} qw(WASQ ASQ CLKQS RIGQ BS CS UPIN IMDI);

map {
    my $exp = $_;
    $exp_total_coarse{$exp} = 0;
    $exp_failed_total_coarse{$exp} = 0;
} qw(WASQ ASQ CLKQS RIGQ BS CS UPIN IMDI);


#---------------predictor------------------
while (my $line = <$predictor_wise>) {
    chomp $line;

    my $qs_kpi;
    my $ovlexp;
    my @segs;

    my @exps = ();

    if ($line =~ m/qs_kpi:(\S+)\ ovlexp:(.*)$/xms) {
        $qs_kpi = $1;
        $ovlexp = $2;

        @segs = split(/#/, $ovlexp) if defined $ovlexp;
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
                        if (!defined $ovlexp || $ovlexp eq '') {
                            $exp = 'WASQ';
                            $exp_total{$exp} = $exp_total{$exp} + $y;
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
                        if (!defined $ovlexp || $ovlexp eq '') {
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
                    } elsif ($x == '5') {
                        if (!defined $ovlexp || $ovlexp eq '') {
                            $exp = 'RIGQ';
                            $exp_total{$exp} = $exp_total{$exp} + $y;
                        } else {
                            $exp = first { $_ =~ m/RIGQ$/x } @exps;
                            if (defined $exp) {
                                $exp_total{$exp} = $exp_total{$exp} + $y;
                            } else {
                                $exp = 'RIGQ';
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

        @segs = split(/#/, $ovlexp) if defined $ovlexp;
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
                        if (!defined $ovlexp || $ovlexp eq '') {
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
                        if (!defined $ovlexp || $ovlexp eq '') {
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
                    } elsif ($x == '3') {
                        if (!defined $ovlexp || $ovlexp eq '') {
                            $exp = 'LRQ';
                            $exp_total{$exp} = $exp_total{$exp} + $y;
                        } else {
                            $exp = first { $_ =~ m/LRQ$/x } @exps;
                            if (defined $exp) {
                                $exp_total{$exp} = $exp_total{$exp} + $y;
                                #$exp_failed_total{$exp} = $exp_failed_total{$exp} + $z;
                            } else {
                                $exp = 'LRQ';
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

    $havest_loss += 1 if $line =~ m/received\ error\ pack\ from\ predictor,\ err_code\(2004\)/xms;

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
    print $predictor_output "IMAS_$exp" . ":" . $exp_total{$exp}, "\t" if $exp =~ m/Q[S]?/x;
} keys %exp_total;

map {
    my $exp = $_;
    print $predictor_output "IMAS_$exp" . "_FAILED:" . $exp_failed_total{$exp}, "\t" if $exp =~ m/Q[S]?/x;
} keys %exp_failed_total;

print $predictor_output "IMAS_PREDICTOR" . ":" . $imas_predictor, "\t";
print $predictor_output "IMAS_HAVEST" . "_LOSS:" . $havest_loss, "\t";

print $predictor_output "\n";

close $predictor_output;


#---------------imbs-----------------------
while (my $line = <$imbs_wise>) {
    chomp $line;

    my $bsrn = 0;
    my $ovlexp;
    my @segs;

    my @exps = ();

    if ($line =~ m/bsrn:(?:\S+)\ ovlexp:(.*)$/xms) {
        $bsrn = 1;
        $ovlexp = $1;

        @segs = split(/#/, $ovlexp) if defined $ovlexp;
        if (@segs && scalar(@segs) > 0) {
            for my $seg (@segs) {
                if (exists $ovlexp{$seg}) {
                    push @exps, $ovlexp{$seg};
                }
            }
        }

        my $exp;

        if (!defined $ovlexp || $ovlexp eq '') {
            $exp = 'BS';
            $exp_total{$exp} += $bsrn;
        } else {
            $exp = first { $_ =~ m/BS$/x } @exps;
            if (defined $exp) {
                $exp_total{$exp} += $bsrn;
            } else {
                $exp = 'BS';
                $exp_total{$exp} += $bsrn;
            }
        }
    }
}

while (my $line = <$imbs_pc>) {
    chomp $line;

    my $bsrn = 0;
    my $ovlexp;
    my @segs;

    my @exps = ();

    if ($line =~ m/bsrn:(?:\S+)\ ovlexp:(.*)$/xms) {
        $bsrn = 1;
        $ovlexp = $1;

        @segs = split(/#/, $ovlexp) if defined $ovlexp;
        if (@segs && scalar(@segs) > 0) {
            for my $seg (@segs) {
                if (exists $ovlexp{$seg}) {
                    push @exps, $ovlexp{$seg};
                }
            }
        }

        my $exp;

        if (!defined $ovlexp || $ovlexp eq '') {
            $exp = 'BS';
            $exp_total{$exp} += $bsrn;
        } else {
            $exp = first { $_ =~ m/BS$/x } @exps;
            if (defined $exp) {
                $exp_total{$exp} += $bsrn;
            } else {
                $exp = 'BS';
                $exp_total{$exp} += $bsrn;
            }
        }
    }
}

while (my $line = <$imbs_wf>) {
    chomp $line;

    if ($line =~ m/platform\[(.*)\]/xms) {
        my $exp = $1;
        $exp =~ s/^bs//xms;
        $exp =~ s/n$//xms;
        $exp =~ s/_//xms;
        $exp = $exp . 'bs';
        $exp = uc $exp;
        if ($line =~ m/bs\ group.*\ is\ not\ valid/x) {
            $exp = $exp . "_SINGLE_GROUP";
            $exp_failed_total{$exp} = (exists $exp_failed_total{$exp} ? $exp_failed_total{$exp} : 0) + 1;
        }
        elsif ($line =~ m/all\ bs\ groups\ failed/x) {
            $exp = $exp . "_DOUBLE_GROUP";
            $exp_failed_total{$exp} = (exists $exp_failed_total{$exp} ? $exp_failed_total{$exp} : 0) + 1;
        }
    }
}

print $imbs_output $yesterday_str . "\t";

map {
    my $exp = $_;
    print $imbs_output "IMAS_$exp" . ":" . $exp_total{$exp}, "\t" if $exp =~ m/BS$/x;
} keys %exp_total;

map {
    my $exp = $_;

    my $exp_group = $exp . "_SINGLE_GROUP";
    $exp_failed_total{$exp_group} = 0 unless exists $exp_failed_total{$exp_group};
    print $imbs_output "IMAS_$exp_group" . "_FAILED:" . $exp_failed_total{$exp_group}, "\t" if $exp =~ m/BS$/x;

    $exp_group = $exp . "_DOUBLE_GROUP";
    $exp_failed_total{$exp_group} = 0 unless exists $exp_failed_total{$exp_group};
    print $imbs_output "IMAS_$exp_group" . "_FAILED:" . $exp_failed_total{$exp_group}, "\t" if $exp =~ m/BS$/x;
} keys %exp_failed_total;

print $imbs_output "\n";

close $imbs_output;

#---------------imcs-----------------------
while (my $line = <$imcs_wise>) {
    chomp $line;

    my $csfn = 0;
    my $ovlexp;
    my @segs;

    my @exps = ();

    if ($line =~ m/csfn:(?:\S+)\ ovlexp:(.*)$/xms) {
        $csfn = 1;
        $ovlexp = $1;

        @segs = split(/#/, $ovlexp) if defined $ovlexp;
        if (@segs && scalar(@segs) > 0) {
            for my $seg (@segs) {
                if (exists $ovlexp{$seg}) {
                    push @exps, $ovlexp{$seg};
                }
            }
        }

        my $exp;

        if (!defined $ovlexp || $ovlexp eq '') {
            $exp = 'CS';
            $exp_total{$exp} += $csfn;
        } else {
            $exp = first { $_ =~ m/CS$/x } @exps;
            if (defined $exp) {
                $exp_total{$exp} += $csfn;
            } else {
                $exp = 'CS';
                $exp_total{$exp} += $csfn;
            }
        }
    }
}

while (my $line = <$imcs_pc>) {
    chomp $line;

    my $csfn = 0;
    my $ovlexp;
    my @segs;

    my @exps = ();

    if ($line =~ m/csfn:(?:\S+)\ ovlexp:(.*)$/xms) {
        $csfn = 1;
        $ovlexp = $1;

        @segs = split(/#/, $ovlexp) if defined $ovlexp;
        if (@segs && scalar(@segs) > 0) {
            for my $seg (@segs) {
                if (exists $ovlexp{$seg}) {
                    push @exps, $ovlexp{$seg};
                }
            }
        }

        my $exp;

        if (!defined $ovlexp || $ovlexp eq '') {
            $exp = 'CS';
            $exp_total{$exp} += $csfn;
        } else {
            $exp = first { $_ =~ m/CS$/x } @exps;
            if (defined $exp) {
                $exp_total{$exp} += $csfn;
            } else {
                $exp = 'CS';
                $exp_total{$exp} += $csfn;
            }
        }
    }
}

while (my $line = <$imcs_wf>) {
    chomp $line;

    if ($line =~ m/interact\ with\ ras([a-z0-9]+)\ failed/x) {
        my $exp = $1;
        $exp =~ s/^ras//xms;
        $exp =~ s/n$//xms;
        $exp =~ s/_//xms;
        $exp = $exp . 'cs';
        $exp = uc $exp;
        $exp_failed_total{$exp} = (exists $exp_failed_total{$exp} ? $exp_failed_total{$exp} : 0) + 1;
    }
}

print $imcs_output $yesterday_str . "\t";

map {
    my $exp = $_;
    print $imcs_output "IMAS_$exp" . ":" . $exp_total{$exp}, "\t" if $exp =~ m/CS$/x;
} keys %exp_total;

map {
    my $exp = $_;
    print $imcs_output "IMAS_$exp" . "_FAILED:" . $exp_failed_total{$exp}, "\t" if $exp =~ m/CS$/x;
} keys %exp_failed_total;

print $imcs_output "\n";

close $imcs_output;


my $module;
my $output;
my $fd;
#---------------imdi-----------------------
$module = 'IMDI';
$output = $imdi_output;
$fd = $imdi_wf;

while (my $line = <$fd>) {
    chomp $line;

    $exp_failed_total{$module} = (exists $exp_failed_total{$module} ? $exp_failed_total{$module} : 0) + 1;
}

print $output $yesterday_str . "\t";

print $output "IMAS_$module" . ":" . $imas_flow, "\t";

print $output "IMAS_$module" . "_FAILED:" . (exists $exp_failed_total{$module} ? $exp_failed_total{$module} : 0);

print $output "\n";

close $output;


#---------------upin-----------------------
$module = 'UPIN';
$output = $upin_output;
$fd = $upin_wf;

while (my $line = <$fd>) {
    chomp $line;

    $exp_failed_total{$module} = (exists $exp_failed_total{$module} ? $exp_failed_total{$module} : 0) + 1;
}

print $output $yesterday_str . "\t";

print $output "IMAS_$module" . ":" . $imas_flow, "\t";

print $output "IMAS_$module" . "_FAILED:" . (exists $exp_failed_total{$module} ? $exp_failed_total{$module} : 0);

print $output "\n";

close $output;


#---------------VSQ------------------------
$module = 'VSQ';
$output = $vsq_output;
$fd = $vsq_wf;

while (my $line = <$fd>) {
    chomp $line;

    $exp_failed_total{$module} = (exists $exp_failed_total{$module} ? $exp_failed_total{$module} : 0) + 1;
}

print $output $yesterday_str . "\t";

print $output "IMAS_$module" . ":" . $imas_flow, "\t";

print $output "IMAS_$module" . "_FAILED:" . (exists $exp_failed_total{$module} ? $exp_failed_total{$module} : 0);

print $output "\n";

close $output;
