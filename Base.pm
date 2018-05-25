package Koha::Illbackends::FreeForm::Base;

# Copyright PTFS Europe 2014, 2018
#
# This file is part of Koha.
#
# Koha is free software; you can redistribute it and/or modify it under the
# terms of the GNU General Public License as published by the Free Software
# Foundation; either version 3 of the License, or (at your option) any later
# version.
#
# Koha is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
# A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with Koha; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

use Modern::Perl;
use DateTime;
use Koha::Illrequests;
use Koha::Illrequestattribute;

=head1 NAME

Koha::Illrequest::Backend::FreeForm::Base - Koha ILL Backend: FreeForm

=head1 SYNOPSIS

Koha ILL implementation for the "FreeForm" backend.

=head1 DESCRIPTION

=head2 Overview

We will be providing the Abstract interface which requires we implement the
following methods:
- create        -> initial placement of the request for an ILL order
- confirm       -> confirm placement of the ILL order (No-op in FreeForm)
- cancel        -> request an already 'confirm'ed ILL order be cancelled
- status_graph  -> return a hashref of additional statuses
- name          -> return the name of this backend
- metadata      -> return mapping of fields from requestattributes

=head2 On the FreeForm backend

The FreeForm backend is a simple backend that is supposed to act as a
fallback.  It provides the end user with some mandatory fields in a form as
well as the option to enter additional fields with arbitrary names & values.

=head1 API

=head2 Class Methods

=cut

=head3 new

  my $backend = Koha::Illrequest::Backend::FreeForm->new;

=cut

sub new {

    # -> instantiate the backend
    my ($class) = @_;
    my $self = {};
    bless( $self, $class );
    return $self;
}

=head3 name

Return the name of this backend.

=cut

sub name {
    return "FreeForm";
}

=head3 capabilities

    $capability = $backend->capabilities($name);

Return the sub implementing a capability selected by NAME, or 0 if that
capability is not implemented.

=cut

sub capabilities {
    my ( $self, $name ) = @_;
    my ($query) = @_;
    my $capabilities = {

        # Get the requested partner email address(es)
        get_requested_partners => sub { _get_requested_partners(@_); },

        # Set the requested partner email address(es)
        set_requested_partners => sub { _set_requested_partners(@_); }
    };
    return $capabilities->{$name};
}

=head3 metadata

Return a hashref containing canonical values from the key/value
illrequestattributes store. We may want to ignore certain values
that we do not consider to be metadata

=cut

sub metadata {
    my ( $self, $request ) = @_;
    my $attrs       = $request->illrequestattributes;
    my $metadata    = {};
    my @ignore      = ('requested_partners');
	my $core_fields = _get_core_fields();
    while ( my $attr = $attrs->next ) {
        my $type = $attr->type;
        if ( !grep { $_ eq $type } @ignore ) {
            my $name;
            $name = $core_fields->{$type} || ucfirst($type);
            $metadata->{$name} = $attr->value;
        }
    }
    return $metadata;
}

=head3 status_graph

This backend provides no additional actions on top of the core_status_graph.

=cut

sub status_graph {
    return {
        MIG => {
            prev_actions =>
              [ 'NEW', 'REQ', 'GENREQ', 'REQREV', 'QUEUED', 'CANCREQ', ],
            id             => 'MIG',
            name           => 'Backend Migration',
            ui_method_name => 'Switch provider',
            method         => 'migrate',
            next_actions   => [],
            ui_method_icon => 'fa-search',
        },
        EDITITEM => {
            prev_actions   => [ 'NEW' ],
            id             => 'EDITITEM',
            name           => 'Edited item metadata',
            ui_method_name => 'Edit item metadata',
            method         => 'edititem',
            next_actions   => [],
            ui_method_icon => 'fa-edit',
        },

    };
}

=head3 create

  my $response = $backend->create({ params => $params });

We just want to generate a form that allows the end-user to associate key
value pairs in the database.

=cut

sub create {
    my ( $self, $params ) = @_;
    my $other = $params->{other};
    my $stage = $other->{stage};
    if ( !$stage || $stage eq 'init' ) {

        # We simply need our template .INC to produce a form.
        return {
            error   => 0,
            status  => '',
            message => '',
            method  => 'create',
            stage   => 'form',
            value   => $params,
        };
    }
    elsif ( $stage eq 'form' ) {

        # We may be recieving a submitted form due to an additional
        # custom field being added or deleted, so check for that
        if ( defined $other->{'add_new_custom'} ) {
            my ( $custom_keys, $custom_vals ) =
              _get_custom( $other->{'custom_key'}, $other->{'custom_value'} );
            push @{$custom_keys}, '---';
            push @{$custom_vals}, '---';
            $other->{'custom_key_del'}   = join "\t", @{$custom_keys};
            $other->{'custom_value_del'} = join "\t", @{$custom_vals};
            my $result = {
                status  => "",
                message => "",
                error   => 0,
                value   => $params,
                method  => "create",
                stage   => "form",
            };
            return $result;
        }
        elsif ( defined $other->{'custom_delete'} ) {
            my $delete_idx = $other->{'custom_delete'};
            my ( $custom_keys, $custom_vals ) =
              _get_custom( $other->{'custom_key'}, $other->{'custom_value'} );
            splice @{$custom_keys}, $delete_idx, 1;
            splice @{$custom_vals}, $delete_idx, 1;
            $other->{'custom_key_del'}   = join "\t", @{$custom_keys};
            $other->{'custom_value_del'} = join "\t", @{$custom_vals};
            my $result = {
                status  => "",
                message => "",
                error   => 0,
                value   => $params,
                method  => "create",
                stage   => "form",
            };
            return $result;
        }

        # Received completed details of form.  Validate and create request.
        ## Validate
        my ( $brw_count, $brw ) =
          _validate_borrower( $other->{'cardnumber'} );
        my $result = {
            status  => "",
            message => "",
            error   => 1,
            value   => {},
            method  => "create",
            stage   => "form",
        };
        my $failed = 0;
        if ( !$other->{'title'} ) {
            $result->{status} = "missing_title";
            $result->{value}  = $params;
            $failed           = 1;
        }
        elsif ( !$other->{'type'} ) {
            $result->{status} = "missing_type";
            $result->{value}  = $params;
            $failed           = 1;
        }
        elsif ( !$other->{'branchcode'} ) {
            $result->{status} = "missing_branch";
            $result->{value}  = $params;
            $failed           = 1;
        }
        elsif ( !Koha::Libraries->find( $other->{'branchcode'} ) ) {
            $result->{status} = "invalid_branch";
            $result->{value}  = $params;
            $failed           = 1;
        }
        elsif ( $brw_count == 0 ) {
            $result->{status} = "invalid_borrower";
            $result->{value}  = $params;
            $failed           = 1;
        }
        elsif ( $brw_count > 1 ) {

            # We must select a specific borrower out of our options.
            $params->{brw}   = $brw;
            $result->{value} = $params;
            $result->{stage} = "borrowers";
            $result->{error} = 0;
            $failed          = 1;
        }
        if ($failed) {
            my ( $custom_keys, $custom_vals ) =
              _get_custom( $other->{'custom_key'}, $other->{'custom_value'} );
            $other->{'custom_key_del'}   = join "\t", @{$custom_keys};
            $other->{'custom_value_del'} = join "\t", @{$custom_vals};
            return $result;
        }

        ## Create request

        # ...Populate Illrequest
        my $request = $params->{request};
        $request->borrowernumber( $brw->borrowernumber );
        $request->branchcode( $params->{other}->{branchcode} );
        $request->status('NEW');
        $request->backend( $params->{other}->{backend} );
        $request->placed( DateTime->now );
        $request->updated( DateTime->now );
        $request->store;

        # ...Populate Illrequestattributes
        # generate $request_details
        my $request_details = _get_request_details($params, $other);
        while ( my ( $type, $value ) = each %{$request_details} ) {
            if ($value && length $value > 0) {
                Koha::Illrequestattribute->new(
                    {
                        illrequest_id => $request->illrequest_id,
                        type          => $type,
                        value         => $value,
                        readonly      => 0
                    }
                )->store;
            }
        }

        ## -> create response.
        return {
            error   => 0,
            status  => '',
            message => '',
            method  => 'create',
            stage   => 'commit',
            next    => 'illview',
            value   => $request_details,
        };
    }
    else {
        # Invalid stage, return error.
        return {
            error   => 1,
            status  => 'unknown_stage',
            message => '',
            method  => 'create',
            stage   => $params->{stage},
            value   => {},
        };
    }
}

=head3 edititem

=cut

sub edititem {
    my ( $self, $params ) = @_;

    # Don't allow editing of submitted requests
    $response->{method} = 'illlist' if $params->{request}->status ne 'NEW';

    my $other = $params->{other};
    my $stage = $other->{stage};
    if ( !$stage || $stage eq 'init' ) {

		my $attrs = $params->{request}->illrequestattributes->unblessed;
		my $core = _get_core_fields();
		# We need to identify which parameters are custom, and pass them
		# to the template in a predefined form
		my $custom_keys = [];
		my $custom_vals = [];
		foreach my $attr(@{$attrs}) {
			if (!$core->{$attr->{type}}) {
				push @{$custom_keys}, $attr->{type};
				push @{$custom_vals}, $attr->{value};
			} else {
				$other->{$attr->{type}} = $attr->{value};
			}
		}
		$other->{'custom_key_del'}   = join "\t", @{$custom_keys};
		$other->{'custom_value_del'} = join "\t", @{$custom_vals};
        # Pass everything back to the template
        return {
            error   => 0,
            status  => '',
            message => '',
            method  => 'edititem',
            stage   => 'form',
            value   => $params,
        };
    }
    elsif ( $stage eq 'form' ) {
		# We don't want the request ID param getting any further
		delete $other->{illrequest_id};

		my $result = {
			status  => "",
			message => "",
			error   => 1,
			value   => {},
			method  => "edititem",
			stage   => "form",
		};
        # Received completed details of form.  Validate and create request.
        ## Validate
        my $failed = 0;
        if ( !$other->{'title'} ) {
            $result->{status} = "missing_title";
            $result->{value}  = $params;
            $failed           = 1;
        }
        elsif ( !$other->{'type'} ) {
            $result->{status} = "missing_type";
            $result->{value}  = $params;
            $failed           = 1;
		}
        elsif ( !$other->{'author'} ) {
            $result->{status} = "missing_author";
            $result->{value}  = $params;
            $failed           = 1;
        }
        if ($failed) {
            my ( $custom_keys, $custom_vals ) =
              _get_custom( $other->{'custom_key'}, $other->{'custom_value'} );
            $other->{'custom_key_del'}   = join "\t", @{$custom_keys};
            $other->{'custom_value_del'} = join "\t", @{$custom_vals};
            return $result;
        }

        ## Update request

        # ...Update Illrequest
        my $request = $params->{request};
        $request->updated( DateTime->now );
        $request->store;

        # ...Populate Illrequestattributes
        # generate $request_details
        my $request_details = _get_request_details($params, $other);
        # We do this with a 'dump all and repopulate approach' inside
        # a transaction, easier than catering for create, update & delete
        my $dbh    = C4::Context->dbh;
        my $schema = Koha::Database->new->schema;
        $schema->txn_do(
            sub{
                # Delete all existing attributes for this request
                $dbh->do( q|
                    DELETE FROM illrequestattributes WHERE illrequest_id=?
                |, undef, $request->id);
                # Insert all current attributes for this request
                foreach my $attr(%{$request_details}) {
                    my $value = $request_details->{$attr};
                    if ($value && length $value > 0){
                        my @bind = ($request->id, $attr, $value, 0);
                        $dbh->do ( q|
                            INSERT INTO illrequestattributes
                            (illrequest_id, type, value, readonly) VALUES
                            (?, ?, ?, ?)
                        |, undef, @bind);
                    }
                }
            }
        );

        ## -> create response.
        return {
            error   => 0,
            status  => '',
            message => '',
            method  => 'create',
            stage   => 'commit',
            next    => 'illview',
            value   => $request_details,
        };
    }
    else {
        # Invalid stage, return error.
        return {
            error   => 1,
            status  => 'unknown_stage',
            message => '',
            method  => 'create',
            stage   => $params->{stage},
            value   => {},
        };
    }
}


=head3 confirm

  my $response = $backend->confirm({ params => $params });

Confirm the placement of the previously "selected" request (by using the
'create' method).

In the FreeForm backend we only want to display a bit of text to let staff
confirm that they have taken the steps they need to take to "confirm" the
request.

=cut

sub confirm {
    my ( $self, $params ) = @_;
    my $stage = $params->{other}->{stage};
    if ( !$stage || $stage eq 'init' ) {

        # We simply need our template .INC to produce a text block.
        return {
            method => 'confirm',
            stage  => 'confirm',
            value  => $params,
        };
    }
    elsif ( $stage eq 'confirm' ) {
        my $request = $params->{request};
        $request->orderid( $request->illrequest_id );
        $request->status("REQ");
        $request->store;

        # ...then return our result:
        return {
            method => 'confirm',
            stage  => 'commit',
            next   => 'illview',
            value  => {},
        };
    }
    else {
        # Invalid stage, return error.
        return {
            error   => 1,
            status  => 'unknown_stage',
            message => '',
            method  => 'confirm',
            stage   => $params->{stage},
            value   => {},
        };
    }
}

=head3 cancel

  my $response = $backend->cancel({ params => $params });

We will attempt to cancel a request that was confirmed.

In the FreeForm backend this simply means displaying text to the librarian
asking them to confirm they have taken all steps needed to cancel a confirmed
request.

=cut

sub cancel {
    my ( $self, $params ) = @_;
    my $stage = $params->{other}->{stage};
    if ( !$stage || $stage eq 'init' ) {

        # We simply need our template .INC to produce a text block.
        return {
            method => 'cancel',
            stage  => 'confirm',
            value  => $params,
        };
    }
    elsif ( $stage eq 'confirm' ) {
        $params->{request}->status("REQREV");
        $params->{request}->orderid(undef);
        $params->{request}->store;
        return {
            method => 'cancel',
            stage  => 'commit',
            next   => 'illview',
            value  => $params,
        };
    }
    else {
        # Invalid stage, return error.
        return {
            error   => 1,
            status  => 'unknown_stage',
            message => '',
            method  => 'cancel',
            stage   => $params->{stage},
            value   => {},
        };
    }
}

=head3 migrate

Migrate a request into or out of this backend.

=cut

sub migrate {
    my ( $self, $params ) = @_;
    my $other = $params->{other};

    my $stage = $other->{stage};
    my $step  = $other->{step};

    # Recieve a new request from another backend and suppliment it with
    # anything we require specifically for this backend.
    if ( !$stage || $stage eq 'immigrate' ) {
        my $original_request =
          Koha::Illrequests->find( $other->{illrequest_id} );
        my $new_request = $params->{request};
        $new_request->borrowernumber( $original_request->borrowernumber );
        $new_request->branchcode( $original_request->branchcode );
        $new_request->status('NEW');
        $new_request->backend( $self->name );
        $new_request->placed( DateTime->now );
        $new_request->updated( DateTime->now );
        $new_request->store;

        my @default_attributes = (
            qw/title type author year volume isbn issn article_title article_author aritlce_pages/
        );
        my $original_attributes =
          $original_request->illrequestattributes->search(
            { type => { '-in' => \@default_attributes } } );

        my $request_details =
          { map { $_->type => $_->value } ( $original_attributes->as_list ) };
        $request_details->{migrated_from} = $original_request->illrequest_id;
        while ( my ( $type, $value ) = each %{$request_details} ) {
            Koha::Illrequestattribute->new(
                {
                    illrequest_id => $new_request->illrequest_id,
                    type          => $type,
                    value         => $value,
                }
            )->store;
        }

        return {
            error   => 0,
            status  => '',
            message => '',
            method  => 'migrate',
            stage   => 'commit',
            next    => 'emigrate',
            value   => $params,
        };
    }

    # Cleanup any outstanding work, close the request.
    elsif ( $stage eq 'emigrate' ) {
        my $request = $params->{request};

        # Just cancel the original request now it's been migrated away
        $request->status("REQREV");
        $request->orderid(undef);
        $request->store;

        return {
            error   => 0,
            status  => '',
            message => '',
            method  => 'migrate',
            stage   => 'commit',
            value   => $params,
        };
    }
}

## Helpers

=head3 _get_requested_partners

=cut

sub _get_requested_partners {

    # Take a request and retrieve an Illrequestattribute with
    # the type 'requested_partners'.
    my ($args) = @_;
    my $where = {
        illrequest_id => $args->{request}->id,
        type          => 'requested_partners'
    };
    my $res = Koha::Illrequestattributes->find($where);
    return ($res) ? $res->value : undef;
}

=head3 _set_requested_partners

=cut

sub _set_requested_partners {

    # Take a request and set an Illrequestattribute on it
    # detailing the email address(es) of the requested
    # partner(s). We replace any existing value since, by
    # the time we get to this stage, any previous request
    # from partners would have had to be cancelled
    my ($args) = @_;
    my $where = {
        illrequest_id => $args->{request}->id,
        type          => 'requested_partners'
    };
    Koha::Illrequestattributes->search($where)->delete();
    Koha::Illrequestattribute->new(
        {
            illrequest_id => $args->{request}->id,
            type          => 'requested_partners',
            value         => $args->{to}
        }
    )->store;
}

=head3 _validate_borrower

=cut

sub _validate_borrower {

    # Perform cardnumber search.  If no results, perform surname search.
    # Return ( 0, undef ), ( 1, $brw ) or ( n, $brws )
    my ($input) = @_;
    my $patrons = Koha::Patrons->new;
    my ( $count, $brw );
    my $query = { cardnumber => $input };

    my $brws = $patrons->search($query);
    $count = $brws->count;
    my @criteria = qw/ surname firstname end /;
    while ( $count == 0 ) {
        my $criterium = shift @criteria;
        return ( 0, undef ) if ( "end" eq $criterium );
        $brws = $patrons->search( { $criterium => $input } );
        $count = $brws->count;
    }
    if ( $count == 1 ) {
        $brw = $brws->next;
    }
    else {
        $brw = $brws;    # found multiple results
    }
    return ( $count, $brw );
}

=head3 _get_custom

=cut

sub _get_custom {

    # Take an string of custom keys and an string
    # of custom values, both delimited by \0 (by CGI)
    # and return an arrayref of each
    my ( $keys, $values ) = @_;
    my @k = defined $keys   ? split( "\0", $keys )   : ();
    my @v = defined $values ? split( "\0", $values ) : ();
    return ( \@k, \@v );
}

=head3 _prepare_custom

=cut

sub _prepare_custom {

    # Take an arrayref of custom keys and an arrayref
    # of custom values, return a hashref of them
    my ( $keys, $values ) = @_;
    my %out = ();
    if ($keys) {
        my @k = split( "\0", $keys );
        my @v = split( "\0", $values );
        %out = map { $k[$_] => $v[$_] } 0 .. $#k;
    }
    return \%out;
}

=head3 _get_request_details

    my $request_details = _get_request_details($params, $other);

Return the illrequestattributes for a given request

=cut

sub _get_request_details {
    my ($params, $other) = @_;

    # Get custom key / values we've been passed
    # Prepare them for addition into the Illrequestattribute object
    my $custom =
        _prepare_custom( $other->{'custom_key'}, $other->{'custom_value'} );

    my $return = {
        %$custom
    };
    my $core = _get_core_fields();
    foreach my $key(%{$core}) {
        $return->{$key} = $params->{other}->{$key};
    }

    return $return;
}

=head3 _get_core_fields

Return a hashref of core fields

=cut

sub _get_core_fields {
    return {
        type           => 'Type',
        title          => 'Title',
        author         => 'Author',
        isbn           => 'ISBN',
        issn           => 'ISSN',
        part_edition   => 'Part / Edition',
        volume         => 'Volume',
        year           => 'Year',
        article_title  => 'Part Title',
        article_author => 'Part Author',
        article_pages  => 'Part Pages',
    };
}

=head1 AUTHORS

Alex Sassmannshausen <alex.sassmannshausen@ptfs-europe.com>
Martin Renvoize <martin.renvoize@ptfs-europe.com>
Andrew Isherwood <andrew.isherwood@ptfs-europe.com>

=cut

1;
