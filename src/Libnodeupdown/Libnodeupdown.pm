package Libnodeupdown;

use 5.006;
use strict;
use warnings;
use Carp;

require Exporter;
require DynaLoader;

our @ISA = qw(Exporter DynaLoader);

our $VERSION = '0.01';

bootstrap Libnodeupdown $VERSION;

1;

__END__

# Below is stub documentation for your module. You better edit it!
# Okie Dokie Dookee
 
=head1 NAME

Libnodeupdown - Perl extension for libnodeupdown

=head1 SYNOPSIS

 use Libnodeupdown;

 Libnodeupdown::NODEUPDOWN_ERR_SUCCESS
 Libnodeupdown::NODEUPDOWN_ERR_NULLHANDLE
 Libnodeupdown::NODEUPDOWN_ERR_OPEN
 Libnodeupdown::NODEUPDOWN_ERR_READ
 Libnodeupdown::NODEUPDOWN_ERR_CONNECT
 Libnodeupdown::NODEUPDOWN_ERR_TIMEOUT
 Libnodeupdown::NODEUPDOWN_ERR_HOSTNAME
 Libnodeupdown::NODEUPDOWN_ERR_ADDRESS
 Libnodeupdown::NODEUPDOWN_ERR_NETWORK
 Libnodeupdown::NODEUPDOWN_ERR_ISLOADED
 Libnodeupdown::NODEUPDOWN_ERR_NOTLOADED
 Libnodeupdown::NODEUPDOWN_ERR_OVERFLOW
 Libnodeupdown::NODEUPDOWN_ERR_PARAMETERS
 Libnodeupdown::NODEUPDOWN_ERR_NULLPTR
 Libnodeupdown::NODEUPDOWN_ERR_OUTMEM
 Libnodeupdown::NODEUPDOWN_ERR_NOTFOUND
 Libnodeupdown::NODEUPDOWN_ERR_GENDERS
 Libnodeupdown::NODEUPDOWN_ERR_GANGLIA
 Libnodeupdown::NODEUPDOWN_ERR_HOSTLIST
 Libnodeupdown::NODEUPDOWN_ERR_MAGIC
 Libnodeupdown::NODEUPDOWN_ERR_INTERNAL
 Libnodeupdown::NODEUPDOWN_ERR_ERRNUMRANGE

 Libnodeupdown::NODEUPDOWN_TIMEOUT_LEN
 Libnodeupdown::NODEUPDOWN_CONF_FILE   

 $handle = Libnodeupdown->nodeupdown_handle_create();

 $handle->nodeupdown_load_data();
 $handle->nodeupdown_load_data($genders_filename,
                               $gmond_hostname,
                               $gmond_ip,
                               $gmond_port,
                               $timeout_len);

 $handle->nodeupdown_errnum()
 $handle->nodeupdown_strerror($errnum)
 $handle->nodeupdown_errormsg()
 $handle->nodeupdown_perror($msg)

 $handle->nodeupdown_get_up_nodes_string();
 $handle->nodeupdown_get_down_nodes_string();

 $handle->nodeupdown_get_up_nodes_list();
 $handle->nodeupdown_get_down_nodes_list();

 $handle->nodeupdown_is_node_up($node);
 $handle->nodeupdown_is_node_down($node);

 $handle->nodeupdown_up_count();
 $handle->nodeupdown_down_count();

=head1 DESCRIPTION

This package provides an OO perl interface to the nodeupdown C API
(see libnodeupdown(3)).  The perl interface is simliar to the
nodeupdown C API, with some necessary changes due to the inherent
differences between C and perl.  Please read the instructions below so
to understand how to use the Libnodeupdown package.

=head2 Initialization

=over 4

=item B<Libnodeupdown-E<gt>nodeupdown_handle_create>

Returns a nodeupdown object on success, undef on error.

=item B<$handle-E<gt>nodeupdown_load_data()>

=item B<$handle-E<gt>nodeupdown_load_data($genders_filename, $gmond_hostname, $gmond_ip, $gmond_port, $timeout_len)>

Loads data from genders and ganglia for the remainder of the
nodeupdown library to use.  To use default values, pass in undef for
any of the specified parameters.  To use all defaults, simply call
nodeupdown_load_data without any parameters.  Returns 0 on success, -1
on error.

=back

=head2 Error Messages

Similarly to the C API, an error code is stored in the nodeupdown object
after an error has occurred.  The following can be used to retrieve
the error code and output information about the error.

=over 4

=item B<$handle-E<gt>nodeupdown_errnum()>

Returns the error code most recently set.

=item B<$handle-E<gt>nodeupdown_strerror($errnum)>

Returns a string describing the error code $errnum.  

=item B<$handle-E<gt>nodeupdown_errormsg()>

Returns a string describing the most recent error.

=item B<$handle-E<gt>nodeupdown_perror([$msg])>

Outputs $msg and a string describing the most recent error to standard
error.  If $msg is not specified, only a description of the most
recent error will be output to standard error.

=back

=head2 Data Functions

=over 4

=item B<$handle-E<gt>nodeupdown_get_up_nodes_string()>

Returns a ranged string of nodes that are up.  Returns undef on error.

=item B<$handle-E<gt>nodeupdown_get_down_nodes_string()>

Returns a ranged string of nodes that are down.  Returns undef on error.

=item B<$handle-E<gt>nodeupdown_get_up_nodes_list()>

Returns a list of nodes that are up.  Returns undef on error. 

=item B<$handle-E<gt>nodeupdown_get_down_nodes_list()>

Returns a list of nodes that are up.  Returns undef on error. 

=item B<$handle-E<gt>nodeupdown_is_node_up($node)>

Returns 1 if the specified node is up, 0 if not, -1 on error.

=item B<$handle-E<gt>nodeupdown_is_node_down($node)>

Returns 1 if the specified node is down, 0 if not, -1 on error.

=item B<$handle-E<gt>nodeupdown_up_count()>

Returns the number of up nodes, -1 on error.

=item B<$handle-E<gt>nodeupdown_down_count()>

Returns the number of down nodes, -1 on error.

=back

=head2 Error Codes/Constants

The same error codes and constants listed in /usr/include/nodeupdown.h
can be accessed through the following functions: 

 Libnodeupdown::NODEUPDOWN_ERR_SUCCESS
 Libnodeupdown::NODEUPDOWN_ERR_NULLHANDLE
 Libnodeupdown::NODEUPDOWN_ERR_OPEN
 Libnodeupdown::NODEUPDOWN_ERR_READ
 Libnodeupdown::NODEUPDOWN_ERR_CONNECT
 Libnodeupdown::NODEUPDOWN_ERR_TIMEOUT
 Libnodeupdown::NODEUPDOWN_ERR_HOSTNAME
 Libnodeupdown::NODEUPDOWN_ERR_ADDRESS
 Libnodeupdown::NODEUPDOWN_ERR_NETWORK
 Libnodeupdown::NODEUPDOWN_ERR_ISLOADED
 Libnodeupdown::NODEUPDOWN_ERR_NOTLOADED
 Libnodeupdown::NODEUPDOWN_ERR_OVERFLOW
 Libnodeupdown::NODEUPDOWN_ERR_PARAMETERS
 Libnodeupdown::NODEUPDOWN_ERR_NULLPTR
 Libnodeupdown::NODEUPDOWN_ERR_OUTMEM
 Libnodeupdown::NODEUPDOWN_ERR_NOTFOUND
 Libnodeupdown::NODEUPDOWN_ERR_GENDERS
 Libnodeupdown::NODEUPDOWN_ERR_GANGLIA
 Libnodeupdown::NODEUPDOWN_ERR_HOSTLIST
 Libnodeupdown::NODEUPDOWN_ERR_MAGIC
 Libnodeupdown::NODEUPDOWN_ERR_INTERNAL
 Libnodeupdown::NODEUPDOWN_ERR_ERRNUMRANGE

 Libnodeupdown::NODEUPDOWN_TIMEOUT_LEN
 Libnodeupdown::NODEUPDOWN_CONF_FILE   

=head1 AUTHOR

Albert Chu E<lt>chu11@llnl.govE<gt>

=head1 SEE ALSO

L<libnodeupdown>

=cut
