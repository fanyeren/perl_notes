#!/usr/bin/env perl

# default status dir: .logfile-tail-status/

use strict;
use warnings FATAL => 'all';

use Getopt::Long ();
use Logfile::Tail ();



sub usage {
    my $exit = shift // 0;

    my $usage = "Usage: $0 [--status=DIR | --status=FILE] logfile | --help\n";

    if ($exit) {
        print STDERR $usage;
    } else {
        print $usage;
    }

    exit $exit;
}


my $status_path;

Getopt::Long::GetOptions('status=s' => \$status_path, 'help' => sub { usage() }) or exit 1;


if (@ARGV !=1) {
    usage(2);
}

my %attr = ();

if (defined $status_path) {
    if (-f $status_path) {
        $attr{status_file} = $status_path;

        if ($status_path =~ m!^/!) {
            $attr{status_dir} = "/";
        } else {
            $attr{status_dir} = ".";
        }
    } else {
        $attr{status_dir} = $status_path;
    }
}


my $logfile_name = shift;

my @the_warning;

$SIG{'__WARN__'} = sub {
    push @the_warning, @_;
};


my $logfile = Logfile::Tail->new(
    $logfile_name,
    'r',
    \%attr,
) or do {
    if (@the_warning) {
        s/\n(.)/$1/g for @the_warning;
        s/\n// for @the_warning;
        print STDERR "Error while reading [$logfile_name]: @the_warning: $!\n";
    } else {
        print STDERR "Error reading [$logfile_name]: $!\n";
    }
    exit 3;
};


while (defined (my $line = $logfile->getline())) {
    print $line;
}

1;
