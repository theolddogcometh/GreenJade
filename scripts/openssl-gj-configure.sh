#!/bin/sh
# SPDX-License-Identifier: MIT OR Apache-2.0
#
# Out-of-tree OpenSSL 3.5.7 Configure for make openssl-gj.
# Writes generated files only under the build dir (never third_party/apache-2.0/openssl).
# Host-libc libcrypto.a is a compile stepping stone; DUT libcgj port is later.
# Keep EC / NIST P-521. System perl (vendor Text-Template is GPL, omitted).
#
# Usage: openssl-gj-configure.sh <src> <builddir>
# Env:   CC PERL  (defaults: cc, perl)
set -eu

fail() {
	printf '%s\n' "openssl-gj: FAIL $*" >&2
	exit 1
}

if [ "${1:-}" = "-h" ] || [ "${1:-}" = "--help" ] || [ "$#" -ne 2 ]; then
	echo "usage: $0 <third_party/apache-2.0/openssl> <build/openssl>" >&2
	exit 2
fi

cc=${CC:-cc}
perl=${PERL:-perl}

if [ ! -d "$1" ]; then
	fail "missing src dir $1"
fi
src=$(CDPATH= cd -- "$1" && pwd)
mkdir -p "$2"
builddir=$(CDPATH= cd -- "$2" && pwd)

if [ "$builddir" = "$src" ]; then
	fail "refusing in-tree Configure (src=builddir=$src)"
fi
case "$builddir" in
"$src"|"$src"/*)
	fail "refusing Configure inside vendor src $src"
	;;
esac

test -f "$src/Configure" || fail "missing $src/Configure"
test -f "$src/LICENSE.txt" || fail "missing $src/LICENSE.txt"
test -f "$src/NOTICE" || fail "missing NOTICE"
grep -q 'Apache License' "$src/LICENSE.txt" || fail "LICENSE.txt is not Apache"
# Empty Text-Template-1.56/lib is an OpenSSL::fallback path shim.
# Fail only if GPL-dual sources (LICENSE / .pm) reappear.
if [ -e "$src/external/perl/Text-Template-1.56/LICENSE" ] \
	|| [ -n "$(find "$src/external/perl/Text-Template-1.56" -name '*.pm' 2>/dev/null | head -1)" ]; then
	fail "GPL Text-Template sources present under $src/external/perl/"
fi

if ! command -v "$perl" >/dev/null 2>&1; then
	fail "no perl ($perl); system perl required (vendor Text-Template omitted)"
fi
if ! command -v "$cc" >/dev/null 2>&1; then
	fail "no C compiler ($cc); host-libc libcrypto.a stepping stone needs cc"
fi
if ! "$perl" -e 'require 5.010;' >/dev/null 2>&1; then
	fail "perl $perl is older than 5.10 (OpenSSL Configure needs 5.10+)"
fi
if ! "$perl" -e 'use Text::Template 1.46;' >/dev/null 2>&1; then
	fail "perl Text::Template 1.46+ missing (vendor GPL copy omitted). Debian/Ubuntu: libtext-template-perl  Fedora: perl-Text-Template"
fi

# EL9 perl-interpreter has no FindBin. Host perl-FindBin is optional;
# our MIT OR Apache-2.0 shim is enough for Configure ($FindBin::Bin).
here=$(CDPATH= cd -- "$(dirname "$0")" && pwd)
export PERL5LIB="$here/openssl-gj-perl${PERL5LIB:+:$PERL5LIB}"
if ! "$perl" -e 'use FindBin; die unless defined $FindBin::Bin && $FindBin::Bin ne "";' >/dev/null 2>&1; then
	fail "FindBin unavailable (need $here/openssl-gj-perl/FindBin.pm or perl-FindBin)"
fi

# Do not inherit kernel/freestanding CFLAGS from a parent make.
unset CFLAGS CPPFLAGS CXXFLAGS LDFLAGS ASFLAGS LDLIBS || true

host_arch=$(uname -m)
extra=
case "$host_arch" in
x86_64)
	target=linux-x86_64
	# Faster / constant-time-ish P-256/P-384/P-521; EC stays on without it.
	extra=enable-ec_nistp_64_gcc_128
	;;
aarch64|arm64)
	target=linux-aarch64
	;;
*)
	fail "unsupported host $host_arch (host-libc stepping stone is linux-x86_64 / linux-aarch64)"
	;;
esac

# no-ec is forbidden. no-shared no-dso no-tests as required.
# no-docs / no-apps: no pod2man, no openssl CLI (libcrypto only).
set -- \
	"$target" \
	-w \
	"--prefix=$builddir/stage" \
	"--openssldir=$builddir/stage/ssl" \
	no-shared \
	no-dso \
	no-tests \
	no-docs \
	no-apps
if [ -n "$extra" ]; then
	set -- "$@" "$extra"
fi

stamp_body=$(
	printf '%s\n' \
		"src=$src" \
		"cc=$cc" \
		"perl=$perl" \
		"target=$target" \
		"args=$*"
)

stamp=$builddir/.gj-openssl-configure-stamp
need_cfg=1
if [ -f "$stamp" ] && [ -f "$builddir/Makefile" ] \
	&& [ -f "$builddir/include/openssl/configuration.h" ]; then
	if [ "$(cat "$stamp")" = "$stamp_body" ]; then
		need_cfg=0
	fi
fi

if [ "$need_cfg" -eq 1 ]; then
	# Reconfigure from a clean out-of-tree dir (never the vendor tree).
	# shellcheck disable=SC2115
	rm -rf "$builddir"
	mkdir -p "$builddir"
	echo "openssl-gj: Configure $target out of tree -> $builddir (EC/P-521 kept)"
	# OpenSSL::fallback requires MODULES.txt dirs during Configure *and*
	# later perl make rules. Empty lib/ only — never unpack GPL sources.
	mkdir -p "$src/external/perl/Text-Template-1.56/lib"
	(
		cd "$builddir"
		CC="$cc" "$perl" "$src/Configure" "$@"
	) || fail "Configure failed"
	printf '%s\n' "$stamp_body" >"$stamp"
else
	echo "openssl-gj: Configure stamp matches; $builddir already configured"
fi

if [ -e "$src/configdata.pm" ] || [ -e "$src/Makefile" ]; then
	fail "Configure wrote generated files into vendor $src"
fi
if [ -e "$src/include/openssl/configuration.h" ]; then
	fail "generated configuration.h in vendor include"
fi
if [ -e "$src/include/openssl/opensslv.h" ]; then
	fail "generated opensslv.h in vendor include"
fi

cfg=$builddir/include/openssl/configuration.h
test -f "$cfg" || fail "missing $cfg after Configure"
# opensslv.h is produced by make build_generated / build_libs, not Configure.
if grep -E '^[[:space:]]*#[[:space:]]*define[[:space:]]+OPENSSL_NO_EC([[:space:]]|$)' "$cfg" >/dev/null 2>&1; then
	fail "OPENSSL_NO_EC in $cfg (need EC / P-521)"
fi
test -f "$builddir/Makefile" || fail "missing $builddir/Makefile after Configure"

echo "openssl-gj: configured $target no-shared no-dso no-tests (host-libc; Dual DoD B OPEN)"
exit 0
