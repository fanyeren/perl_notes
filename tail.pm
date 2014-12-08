package Logfile::Tail;

use strict;
use warnings FATAL => 'all';

our $VERSION = "0.1";

use Symbol ();

use IO::File ();
use Digest::SHA ();
use File::Spec ();

use Fcntl qw(O_RDWR O_CREAT);

use Cwd();


sub _open {
    my ($self, $filename, $offset) = @_;
    my $fh = new IO::File or return;

    $fh->open($filename, '<:raw') or return;

    if ($offset > 0) {
        my $content = $self->_seek_to($fh, $offset);
        return ($fh, $content);
    }
    return ($fh, '');
}

sub _fh {
    *{$_[0]}->{_fh};
}


sub _seek_to {
    my ($self, $fh, $offset) = @_;

    my $offset_start = $offset - $CHECK_LENGTH;

    $offset_start = 0 if $offset_start < 0;

    # TODO
    $fh->seek($offset_start, 0);

    my $buffer = '';
    while ($offset - $offset_start > 0) {
        my $tmp_buffer = '';
        my $read = $fh->read($tmp_buffer, $offset - $offset_start, length($buffer));

        last if not defined $read or $read <= 0;

        $offset_start += $read;
        $buffer .= $tmp_buffer;
    }

    if ($offset_start == $offset) {
        return $buffer;
    } else {
        return;
    }
}

sub _save_offset_to_status {
    my ($self, $offset) = @_;

    my $logfile_name = *$self->{filename};
    my $status_fh = *$self->{status_fh};

    my $checksum = $self->_get_current_checksum();

    $status_fh->seek(0, 0);
    my $archive_text = defined *$self->{archive} ? " archive [@{[ *$self->{archive}]}]" : '';
    $status_fh->printflush("File [$logfile_name]$archive_text offset [$offset] checksum [$checksum]");

    $status_fh->truncate($status_fh->tell);
}

sub _get_current_checksum {
    my $self = shift;
    my $data_length = *$self->{data_length};

    my $data = *$self->{data_array};
    my $i = 0;

    my $digest = new Digest::SHA('sha256');

    if ($data_length > $CHECK_LENGTH) {
        $digest->add(substr($data->[0], $data_length - $CHECK_LENGTH));
        $i++;
    }

    for (; $i <= $#$data, $i++) {
        $digest->add($data->[$i]);
    }

    return $digest->hexdigest();
}


sub _close_status {
    my ($self, $offset) = @_;
    my $status_fh = delete *$self->{status_fh};
    $status_fh->close() if defined $status_fh;
}


sub _getline {
    my $self = shift;
    my $fh = $self->_fh;
    if (defined $fh) {
    } else {
        return undef;
    }
}

sub getline {
    my $self = shift;
    my $ret = $self->_getline();

    no warnings 'uninitialized';

    if (*$self->{opts}{autocommit} == 2) {
        $self->commit();
    }
    return $ret;
}

sub getlines {
    my $self = shift;

    my @out;

    while (1) {
        my $l = $self->_getline();

        if (not defined $l) {
            last;
        }
        push @out, $l;
    }

    no warnings 'uninitialized';
    if (*$self->{opts}{autocommit} == 2) {
        $self->commit();
    }

    @out;
}

sub commit {
    my $self = shift;

    my $fh = *$self->{_fh};

    my $offset = $fh->tell;

    $self->_save_offset_to_status($offset);
}


sub close {
    my $self = shift;

    if (*$self->{opts}->{autocommit}) {
        $self->commit();
    }

    $self->_close_status();

    my $fh = delete *$self->{_fh};
    $fh->close() if defined $fh;
}


sub TIEHANDLE() {
    if (ref $_[0]) {
        return $_[0];
    } else {
        my $class = shift;
        return $class->new(@_);
    }
}

sub READLINE() {
    goto &getlines if wantarray;
    goto &getline;
}

sub CLOSE() {
    my $self->shift;
    $self->close();
}


sub DESTROY() {
    my $self = shift;
    $self->close() if defined *$self->{_fh};
}

1;
