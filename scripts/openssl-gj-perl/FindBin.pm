# SPDX-License-Identifier: MIT OR Apache-2.0
# Copyright (c) 2026 Project GreenJade contributors
#
# Tiny FindBin stand-in for OpenSSL Configure on hosts that split FindBin
# out of perl-interpreter (EL9). Original glue — not Perl's dual-licensed
# FindBin.pm. Host-only; not linked into libcrypto.

package FindBin;

use strict;
use warnings;
use Cwd qw(abs_path);
use File::Basename qw(basename dirname);

our $VERSION = '0.1-gj';
our $Bin;
our $RealBin;
our $Dir;
our $RealDir;
our $Script;
our $RealScript;

BEGIN {
	my $szScript = $0;
	$Script     = $RealScript = basename($szScript);
	my $szDir   = dirname($szScript);
	$szDir      = abs_path($szDir) if defined $szDir && $szDir ne '';
	$Bin = $RealBin = $Dir = $RealDir = $szDir;
}

1;
