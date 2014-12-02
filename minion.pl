use Mojolicious::Lite;

use v5.20;
use experimental 'signatures';

use Email::Sender::Simple qw(sendmail);
use Email::Simple;
use Email::Simple::Creator;
use Email::Sender::Transport::SMTP::TLS;
use Email::Valid;

use Mojo::JSON 'encode_json';

plugin Config => {file => '/opt/minion_email/config'};
plugin Minion => {Pg => app->config->{pg_string}};

app->secrets([app->config->{secret}]);

app->minion->add_task(email => sub ($job, $email) {
    my $mail = Email::Simple->create(
        header => [
            To     => $email,
            From    => 'signup@host.com',
            Subject => "Super Signup Email",
        ],
        body => "Thank you for signing up with us.\nGlobal conquest in an hour\n",
    );

    my $transport = Email::Sender::Transport::SMTP::TLS->new({
            host => app->config->{smtp_host},
            port => app->config->{smtp_port},
            username => app->config->{smtp_user},
            password => app->config->{smtp_pass},
            timeout => 10,
    });

    eval {
        sendmail($mail, {transport => $transport });
    };
    if ($@) {
        $job->app->log->debug("error: $@");
        $job->finish({ status => "error", msg => $@});
    }
    else {
        $job->finish({ status => "success", msg => "Mail to $email sent"});
    }
});

get '/' => 'index';

under (sub {
    my $self = shift;

    return($self->render(json => {status => "error", data => { message => "No JSON found" }})) unless $self->req->json;

    my $username = $self->req->json->{username};
    my $api_key = $self->req->json->{api_key};

    unless ($username) {
        $self->render(json => {status => "error", data => { message => "No username found" }});

        return undef;
    }

    unless ($api_key) {
        $self->render(json => {status => "error", data => { message => "No API Key found" }});

        return undef;
    }

    unless ("fnord" eq $username) {
        $self->render(json => {status => "error", data => { message => "Credentials mis-match" }});

        return undef;
    }

    unless ("68b329da9893e34099c7d8ad5cb9c940" eq $api_key) {
        $self->render(json => {status => "error", data => { message => "Credentials mis-match" }});

        return undef;
    }

    return 1;
});

post '/api/v1/email' => sub ($c) {
    my $email = $c->req->json->{email};

    unless (Email::Valid->address($email)) {
        return($c->render(json => {status => "error", message => "Email does not seem to be valid"}));
    }

    my $jobid = $c->minion->enqueue(email => [$email]);

    return($c->render(json => {status => "success", message => "Email send to $email", jobid => $jobid}));
};

post '/api/v1/status' => sub ($c) {
    my $jobid = $c->req->json->{jobid};

    unless ($jobid =~ m/^\d+$/) {
        return($c->render(json => {status => "error", message => "JobID not an unsigned integer"}));
    }

    my $job = $c->minion->job($jobid);
    if ($job) {
        return($c->render(json => {status => "success", message => $job->info}));
    }
    else {
        return($c->render(json => {status => "error", message => "Job not: found for $jobid"}));
    }
};

post '/api/v1/stats' => sub ($c) {
    my $stats = $c->minion->stats;
    return($c->render(json => {status => "success", message => $stats}));
};

app->start;

__DATA__

@@ index.html.ep

Try:<br>

    POST /email/v1/email
    POST /email/v1/status
