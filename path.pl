#!/usr/bin/env perl
 
use v5.16;
use strict;
use warnings;
 
use Path::Tiny;
use String::Trigram;
 
my @raw  = path('raw.input')->lines( { chomp => 1 } );
my @ed_a = path('a.input'  )->lines( { chomp => 1 } );
my @ed_b = path('b.input'  )->lines( { chomp => 1 } );
 
say "Text A";
say for get_similarity( \@raw, \@ed_a);
 
say "Text B\n";
say for get_similarity( \@raw, \@ed_b);
 
sub get_similarity {
    my $src  = shift;
    my $dest = shift;
 
    my @result;
    for my $s (@$src) {
        my $max_smlty = 0;
        my $max_smlty_item;
        for my $d (@$dest) {
            my $smlty = String::Trigram::compare($s, $d);
            next unless $smlty > $max_smlty;
 
            $max_smlty      = $smlty;
            $max_smlty_item = $d
        }
        push(
            @result,
            sprintf(
                "%-26s -> %-26s : similarity = %.2f",
                $s,
                $max_smlty_item,
                $max_smlty,
            );
        );
    }
 
    return @result;
}
