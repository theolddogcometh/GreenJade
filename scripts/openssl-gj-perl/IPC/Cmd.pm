# SPDX-License-Identifier: MIT OR Apache-2.0
# Copyright (c) 2026 Project GreenJade contributors
#
# Minimal IPC::Cmd for OpenSSL Configure on EL9 (perl-IPC-Cmd split out).
# OpenSSL::config only calls can_run(). Original glue — not Perl's module.

package IPC::Cmd;

use strict;
use warnings;
use Exporter 'import';

our $VERSION = '0.1-gj';
our @EXPORT_OK = qw(can_run);

sub can_run {
	my $szCmd = shift;
	return undef unless defined $szCmd && $szCmd ne '';
	my $szName = $szCmd;
	$szName =~ s/\s.*//;
	return $szName if $szName =~ m{/} && -x $szName;
	for my $szDir (split /:/, ($ENV{PATH} || q{})) {
		next if $szDir eq '';
		return "$szDir/$szName" if -x "$szDir/$szName";
	}
	return undef;
}

1;
