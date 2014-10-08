#!/usr/bin/env perl

use Net::SSH2;

my $ssh2 = Net::SSH2->new();

$ssh2->connect('tc-asp024.tc') or die $!;

#if ($ssh2->auth_keyboard('fizban')) {
if ($ssh2->auth_password('xiahoufeng', 'xxxxxxxx')) {
#if ($ssh2->auth_publickey ('work', '/home/work/.ssh/id_dsa.pub', '/home/work/.ssh/id_dsa')) {
    my $chan = $ssh2->channel();

    $chan->exec('sudo -H -u root /sbin/ip addr');
    _read($chan);

    my $sftp = $ssh2->sftp();
    my $fh = $sftp->open('/etc/passwd') or die "$!";
    #print $_ while <$fh>;
} else {
    print "error! " . $ssh2->error, "\n";
}


sub _read {
    my $handle = shift;
    while (my $line = <$handle>) {
        chomp $line;
        print $line, "\n";
    }
}

