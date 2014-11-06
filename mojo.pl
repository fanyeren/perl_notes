#!/usr/bin/env perl


$stream->on(read => sub {
        my ($stream, $bytes) = @_;

        $buffer .= $bytes;

        return unless $bytes =~ /\012$/;

        my $res = decode_json $buffer;
        my $result = $res->(res)(as);

        $self->render(json => $result);
    }
);
