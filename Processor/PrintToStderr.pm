package Koha::Illbackends::FreeForm::Processor::PrintToStderr;

use Modern::Perl;

use parent qw(Koha::Illrequest::SupplierUpdateProcessor);

sub new {
    my ( $class ) = @_;
    my $self = $class->SUPER::new('backend', 'FreeForm', 'Print to STDERR');
    bless $self, $class;
    return $self;
}

sub run {
    my ( $self, $update, $status ) = @_;
    my $update_body = $update->{update};

    print STDERR $update_body;
    push @{$status->{success}}, 'PRINTED';
}

1;