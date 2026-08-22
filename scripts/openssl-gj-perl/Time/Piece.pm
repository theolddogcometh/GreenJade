# SPDX-License-Identifier: MIT OR Apache-2.0
# Copyright (c) 2026 Project GreenJade contributors
#
# Minimal Time::Piece for OpenSSL unix-Makefile.tmpl on EL9 (perl-Time-Piece
# split out). Implements strptime("%d %b %Y") and strftime. Original glue.

package Time::Piece;

use strict;
use warnings;
use POSIX ();

our $VERSION = '0.1-gj';

my @aMon = qw(Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec);
my %hMon = map { $aMon[$_] => $_ } 0 .. 11;

sub new {
	my ($szClass, $nEpoch) = @_;
	$nEpoch = time unless defined $nEpoch;
	return bless { _nEpoch => $nEpoch }, $szClass;
}

sub localtime {
	my $nEpoch = @_ ? $_[0] : time;
	return __PACKAGE__->new($nEpoch);
}

sub strptime {
	my ($szClass, $sz, $szFmt) = @_;
	die "Time::Piece::strptime: unsupported fmt '$szFmt'"
		unless defined $szFmt && $szFmt eq '%d %b %Y';
	die "Time::Piece::strptime: cannot parse '$sz'"
		unless defined $sz && $sz =~ /^\s*(\d{1,2})\s+([A-Za-z]{3})\s+(\d{4})\s*$/;
	my $nDay = $1;
	my $nMon = $hMon{$2};
	die "Time::Piece::strptime: month '$2'" unless defined $nMon;
	my $nYear = $3 - 1900;
	my $nEpoch = POSIX::mktime(0, 0, 0, $nDay, $nMon, $nYear);
	return $szClass->new($nEpoch);
}

sub strftime {
	my ($pSelf, $szFmt) = @_;
	return POSIX::strftime($szFmt, CORE::localtime($pSelf->{_nEpoch}));
}

sub import {
	my $szCaller = caller;
	no strict 'refs';
	*{"${szCaller}::localtime"} = \&localtime;
	return;
}

1;
