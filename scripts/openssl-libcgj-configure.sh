#!/bin/sh
# SPDX-License-Identifier: MIT OR Apache-2.0
#
# Out-of-tree OpenSSL 3.5.7 Configure against libcgj (make openssl-libcgj).
# Writes generated files only under build/openssl-libcgj (never vendor,
# never host-libc build/openssl). Keep EC / NIST P-521. Never no-ec.
# no-asm + no-autoload-config: DUT OPENSSL_init_crypto hang (linux-x86_64
# with asm / default openssl.cnf load). System perl (vendor Text-Template
# is GPL, omitted). Dual DoD B OPEN.
#
# Usage: openssl-libcgj-configure.sh <src> <builddir>
# Env:   CC PERL  (defaults: cc, perl)
set -eu

fail() {
	printf '%s\n' "openssl-libcgj: FAIL $*" >&2
	exit 1
}

if [ "${1:-}" = "-h" ] || [ "${1:-}" = "--help" ] || [ "$#" -ne 2 ]; then
	echo "usage: $0 <third_party/openssl> <build/openssl-libcgj>" >&2
	exit 2
fi

cc=${CC:-cc}
perl=${PERL:-perl}

here=$(CDPATH= cd -P -- "$(dirname "$0")" && pwd -P)
root=$(CDPATH= cd -P -- "$here/.." && pwd -P)
libcgj_inc=$root/user/libcgj/include
host_openssl=$root/build/openssl
if [ -d "$host_openssl" ]; then
	host_openssl=$(CDPATH= cd -P -- "$host_openssl" && pwd -P)
fi

# Leaf name is the gate: never mkdir/rm host-libc build/openssl.
build_base=$(basename "$2")
if [ "$build_base" != "openssl-libcgj" ]; then
	fail "refusing builddir $2 (must be .../openssl-libcgj; host-libc build/openssl untouched)"
fi

if [ ! -d "$1" ]; then
	fail "missing src dir $1"
fi
src=$(CDPATH= cd -P -- "$1" && pwd -P)

if [ -e "$2" ] && [ ! -d "$2" ]; then
	fail "builddir $2 is not a directory"
fi
mkdir -p "$2"
builddir=$(CDPATH= cd -P -- "$2" && pwd -P)

if [ "$(basename "$builddir")" != "openssl-libcgj" ]; then
	fail "resolved builddir $builddir is not openssl-libcgj (host-libc build/openssl untouched)"
fi
case "$builddir" in
*/openssl-libcgj) ;;
*)
	fail "refusing builddir $builddir (must end in /openssl-libcgj)"
	;;
esac

if [ "$builddir" = "$src" ]; then
	fail "refusing in-tree Configure (src=builddir=$src)"
fi
case "$builddir" in
"$src"|"$src"/*)
	fail "refusing Configure inside vendor src $src"
	;;
esac

if [ "$builddir" = "$host_openssl" ]; then
	fail "refusing to touch host-libc $host_openssl"
fi
case "$builddir" in
"$host_openssl"|"$host_openssl"/*)
	fail "refusing Configure inside host-libc $host_openssl"
	;;
esac

test -d "$libcgj_inc" || fail "missing libcgj include $libcgj_inc"
test -f "$libcgj_inc/stdio.h" || fail "missing $libcgj_inc/stdio.h"

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
	fail "no C compiler ($cc); libcgj libcrypto Configure needs cc"
fi
if ! "$perl" -e 'require 5.010;' >/dev/null 2>&1; then
	fail "perl $perl is older than 5.10 (OpenSSL Configure needs 5.10+)"
fi
if ! "$perl" -e 'use Text::Template 1.46;' >/dev/null 2>&1; then
	fail "perl Text::Template 1.46+ missing (vendor GPL copy omitted). Debian/Ubuntu: libtext-template-perl  Fedora: perl-Text-Template"
fi

# EL9 perl-interpreter has no FindBin. Host perl-FindBin is optional;
# our MIT OR Apache-2.0 shim is enough for Configure ($FindBin::Bin).
export PERL5LIB="$here/openssl-gj-perl${PERL5LIB:+:$PERL5LIB}"
if ! "$perl" -e 'use FindBin; die unless defined $FindBin::Bin && $FindBin::Bin ne "";' >/dev/null 2>&1; then
	fail "FindBin unavailable (need $here/openssl-gj-perl/FindBin.pm or perl-FindBin)"
fi

# Do not inherit kernel/freestanding CFLAGS from a parent make.
unset CFLAGS CPPFLAGS CXXFLAGS LDFLAGS ASFLAGS LDLIBS || true

host_arch=$(uname -m)
extra=
cc_x86=
case "$host_arch" in
x86_64)
	target=linux-x86_64
	# Faster / constant-time-ish P-256/P-384/P-521; EC stays on without it.
	extra=enable-ec_nistp_64_gcc_128
	cc_x86=1
	;;
aarch64|arm64)
	target=linux-aarch64
	;;
*)
	fail "unsupported host $host_arch (libcgj Configure is linux-x86_64 / linux-aarch64)"
	;;
esac

# no-ec is forbidden. no-shared no-dso no-tests no-docs no-apps no-threads.
# no-asm: C only (DUT OPENSSL_init_crypto hang with linux-x86_64 asm).
# no-autoload-config: skip default openssl.cnf at init (no DUT /etc/ssl).
# CC extras: freestanding libcgj (x86_64 also -mno-red-zone -msse2).
set -- \
	"$target" \
	-w \
	"--prefix=$builddir/stage" \
	"--openssldir=$builddir/stage/ssl" \
	no-shared \
	no-dso \
	no-tests \
	no-docs \
	no-apps \
	no-threads \
	no-asm \
	no-autoload-config \
	-std=gnu11 \
	-ffreestanding \
	-fno-builtin \
	-fno-stack-protector \
	"-I$libcgj_inc"
if [ -n "$cc_x86" ]; then
	set -- "$@" -mno-red-zone -msse2
fi
if [ -n "$extra" ]; then
	set -- "$@" "$extra"
fi

case " $* " in
*" no-ec "*)
	fail "no-ec is forbidden (need EC / P-521)"
	;;
esac
case " $* " in
*" no-asm "*) ;;
*)
	fail "no-asm required (DUT OPENSSL_init_crypto hang with asm)"
	;;
esac
case " $* " in
*" no-autoload-config "*) ;;
*)
	fail "no-autoload-config required (skip openssl.cnf at init)"
	;;
esac

stamp_body=$(
	printf '%s\n' \
		"src=$src" \
		"cc=$cc" \
		"perl=$perl" \
		"target=$target" \
		"libcgj_inc=$libcgj_inc" \
		"args=$*"
)

stamp=$builddir/.gj-openssl-libcgj-configure-stamp
need_cfg=1
if [ -f "$stamp" ] && [ -f "$builddir/Makefile" ] \
	&& [ -f "$builddir/include/openssl/configuration.h" ]; then
	if [ "$(cat "$stamp")" = "$stamp_body" ]; then
		need_cfg=0
	fi
fi

host_existed=0
host_snap=
if [ -d "$host_openssl" ]; then
	host_existed=1
	command -v cksum >/dev/null 2>&1 \
		|| fail "cksum required to prove host-libc $host_openssl untouched"
	host_snap=$(
		for f in \
			"$host_openssl/.gj-openssl-configure-stamp" \
			"$host_openssl/Makefile" \
			"$host_openssl/include/openssl/configuration.h"
		do
			if [ -f "$f" ]; then
				cksum "$f"
			fi
		done
	)
fi

rm_libcgj_builddir() {
	# Fail closed: only the openssl-libcgj leaf, never host-libc openssl.
	[ "$(basename "$builddir")" = "openssl-libcgj" ] \
		|| fail "refuse rm -rf $builddir (not openssl-libcgj)"
	case "$builddir" in
	*/openssl-libcgj) ;;
	*)
		fail "refuse rm -rf $builddir (not openssl-libcgj)"
		;;
	esac
	if [ "$builddir" = "$host_openssl" ]; then
		fail "refuse rm -rf host-libc $host_openssl"
	fi
	[ "$builddir" != "/" ] || fail "refuse rm -rf /"
	# shellcheck disable=SC2115
	rm -rf "$builddir"
}

if [ "$need_cfg" -eq 1 ]; then
	# Reconfigure from a clean out-of-tree dir (never vendor, never host-libc).
	rm_libcgj_builddir
	mkdir -p "$builddir"
	echo "openssl-libcgj: Configure $target no-asm no-autoload-config out of tree -> $builddir (EC/P-521 kept; host-libc $host_openssl untouched)"
	# OpenSSL::fallback requires MODULES.txt dirs during Configure *and*
	# later perl make rules. Empty lib/ only — never unpack GPL sources.
	mkdir -p "$src/external/perl/Text-Template-1.56/lib"
	(
		cd "$builddir"
		CC="$cc" "$perl" "$src/Configure" "$@"
	) || fail "Configure failed"
	printf '%s\n' "$stamp_body" >"$stamp"
else
	echo "openssl-libcgj: Configure stamp matches; $builddir already configured"
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

if [ "$host_existed" -eq 1 ]; then
	[ -d "$host_openssl" ] || fail "host-libc $host_openssl disappeared"
	host_now=$(
		for f in \
			"$host_openssl/.gj-openssl-configure-stamp" \
			"$host_openssl/Makefile" \
			"$host_openssl/include/openssl/configuration.h"
		do
			if [ -f "$f" ]; then
				cksum "$f"
			fi
		done
	)
	if [ "$host_now" != "$host_snap" ]; then
		fail "host-libc $host_openssl mutated (must stay untouched)"
	fi
fi

cfg=$builddir/include/openssl/configuration.h
test -f "$cfg" || fail "missing $cfg after Configure"
# opensslv.h is produced by make build_generated / build_libs, not Configure.
if grep -E '^[[:space:]]*#[[:space:]]*define[[:space:]]+OPENSSL_NO_EC([[:space:]]|$)' "$cfg" >/dev/null 2>&1; then
	fail "OPENSSL_NO_EC in $cfg (need EC / P-521)"
fi
if grep -E '^[[:space:]]*#[[:space:]]*define[[:space:]]+OPENSSL_THREADS([[:space:]]|$)' "$cfg" >/dev/null 2>&1; then
	fail "OPENSSL_THREADS in $cfg (need no-threads)"
fi
if ! grep -E '^[[:space:]]*#[[:space:]]*define[[:space:]]+OPENSSL_NO_ASM([[:space:]]|$)' "$cfg" >/dev/null 2>&1; then
	fail "OPENSSL_NO_ASM missing from $cfg (need no-asm)"
fi
if ! grep -E '^[[:space:]]*#[[:space:]]*define[[:space:]]+OPENSSL_NO_AUTOLOAD_CONFIG([[:space:]]|$)' "$cfg" >/dev/null 2>&1; then
	fail "OPENSSL_NO_AUTOLOAD_CONFIG missing from $cfg (need no-autoload-config)"
fi
test -f "$builddir/Makefile" || fail "missing $builddir/Makefile after Configure"

echo "openssl-libcgj: configured $target no-shared no-dso no-tests no-threads no-asm no-autoload-config (libcgj; Dual DoD B OPEN)"
exit 0
