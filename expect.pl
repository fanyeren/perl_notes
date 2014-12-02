#!/usr/bin/env perl

use strict;
use warnings;

use Expect;

my $userid = 'xiahoufeng';
my $userpass = 'userpass';

my $rootpass = 'QyBRKkxywna@$#BB';

my $exp = Expect->new;

$exp->spawn('ssh', 'dev');

$exp->expect(
    10,
    #[ qr/password:/     => sub { my $exp = shift; $exp->send($userpass . "\n"); exp_continue; }],
    [ qr/$userid.*\$/   => sub { my $exp = shift; $exp->send("su\n");     exp_continue; }],
    #[ qr/Password:/     => sub { my $exp = shift; $exp->send($rootpass . "\n"); exp_continue; }],
    [ qr/^root.*/       => sub { my $exp = shift; $exp->send($rootpass . "\n"); exp_continue; }],
    [ qr/root.*#/       => sub { my $exp = shift; $exp->interact(); }],
);

print "$0 exit\n";
