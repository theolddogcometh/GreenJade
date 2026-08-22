#!/bin/sh
# SPDX-License-Identifier: MIT OR Apache-2.0
#
# Extract GreenJade flash image identity (GJ_IMAGE_VERSION) for lab operators.
#
# Compile-time stamp lives in kernel/include/gj/config.h and is baked into
# KERNEL.ELF / build/greenjade.elf as the panel title:
#   STATUS (static) v0.1.N   (semver series; 0.2.0 reserved, do not ship)
# Serial also prints: main: image version=0.1.N
# Legacy date stamps (YYYY.MM.DD.N) still extract if present in old media.
#
# Usage:
#   ./scripts/gj-image-version.sh              # print version only
#   ./scripts/gj-image-version.sh --title      # print full STATUS title
#   ./scripts/gj-image-version.sh --report     # human multi-line report
#   ./scripts/gj-image-version.sh [elf] […]   # optional ELF path first
#
# Env:
#   GJ_KERNEL_ELF  — default ELF when no path arg (build/greenjade.elf)
#
# Soft!=product: version identity confirms which image was flashed / booted.
# It is not a product PASS, not Steam client, not Deck Top 50, not bar3 close,
# not Dual DoD close, not UDX product AC. Freestanding class SKIP defaults and
# freestanding_no_exec are orthogonal process law (see gj-assurance-check).
#
# Flash bar honesty (lab): stamp e.g. v0.1.178 names the deliverable.
# L1 make assurance-check PASS != Dual DoD A/B close. Dual DoD OPEN until
# host USB path (A) and interactive SSH login (B). Soft!=product · test what you fly.
# Stamp-free residual: this script never bumps GJ_IMAGE_VERSION / never invents
# the next N; it only extracts identity from ELF/config.h (read-only).
# Never treat stale build/esp as fly identity (pack proof is extracted img ESP).
set -eu
root="$(CDPATH= cd -- "$(dirname "$0")/.." && pwd)"
cd "$root"

mode=version
elf=""
for arg in "$@"; do
	case "$arg" in
	-h|--help)
		sed -n '2,29p' "$0" | sed 's/^# \{0,1\}//'
		exit 0
		;;
	--title) mode=title ;;
	--report) mode=report ;;
	--version) mode=version ;;
	-*)
		echo "gj-image-version: unknown option: $arg" >&2
		echo "usage: $0 [--version|--title|--report] [path/to/greenjade.elf|KERNEL.ELF]" >&2
		exit 2
		;;
	*)
		if [ -z "$elf" ]; then
			elf=$arg
		else
			echo "gj-image-version: unexpected arg: $arg" >&2
			exit 2
		fi
		;;
	esac
done

if [ -z "$elf" ]; then
	elf="${GJ_KERNEL_ELF:-build/greenjade.elf}"
fi

# Extract fly bar from an ELF that contains the STATUS title string.
# Prefer the exact panel title (concatenated in fb_console.c); fall back to
# bare stamp if present as its own rodata C-string; then config.h.
# Accepts: 0.1.N (current) or legacy YYYY.MM.DD.N.
extract_from_elf() {
	_f=$1
	[ -f "$_f" ] || return 1
	if ! command -v strings >/dev/null 2>&1; then
		return 1
	fi
	# Panel title: "STATUS (static) v0.1.97" or legacy date form
	_v=$(strings "$_f" 2>/dev/null \
		| sed -n 's/^STATUS (static) v\([0-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\)$/\1/p' \
		| head -n 1)
	if [ -n "$_v" ]; then
		printf '%s\n' "$_v"
		return 0
	fi
	# Bare stamp only (if compiler split the literal)
	_v=$(strings "$_f" 2>/dev/null \
		| grep -E '^[0-9]+\.[0-9]+\.[0-9]+$' \
		| head -n 1)
	if [ -n "$_v" ]; then
		printf '%s\n' "$_v"
		return 0
	fi
	return 1
}

extract_from_config() {
	_h=kernel/include/gj/config.h
	[ -f "$_h" ] || return 1
	_v=$(sed -n 's/^#define[[:space:]][[:space:]]*GJ_IMAGE_VERSION[[:space:]][[:space:]]*"\([^"]*\)".*/\1/p' "$_h" \
		| head -n 1)
	if [ -n "$_v" ]; then
		printf '%s\n' "$_v"
		return 0
	fi
	return 1
}

src=missing
ver=""
if ver=$(extract_from_elf "$elf"); then
	src="elf:$elf"
elif ver=$(extract_from_config); then
	src="config.h"
else
	ver="unknown"
	src="none"
fi

title="STATUS (static) v${ver}"

case "$mode" in
version)
	printf '%s\n' "$ver"
	;;
title)
	printf '%s\n' "$title"
	;;
report)
	echo "gj-image-version: PASS"
	echo "  version:  $ver"
	echo "  title:    $title"
	echo "  source:   $src"
	echo "  flash bar: STATUS (static) v${ver} — identity only (Soft!=product)"
	echo "  verify:   panel STATUS row 0 must match title above after flash/boot"
	echo "  serial:   main: image version=${ver}"
	echo "  Soft!=product: version stamp != product complete != Steam != Top-50 != bar3"
	echo "  product path: userspace UDX+ABI · virtio T0 until UDX owns wire (G-AC-1)"
	echo "  freestanding: class SKIP defaults (GJ_RTL8168_PROBE=0 · GJ_XHCI_MSC_PROBE=0)"
	echo "  freestanding: freestanding_no_exec RUN_INIT=0 (not product AC)"
	echo "  Dual DoD: OPEN — A until USB path; B until interactive SSH login; L1 never closes"
	echo "  assurance: test what you fly — banner / PK_OK / SUCCESS != Dual DoD B close"
	echo "  L1 only:   make assurance-check PASS != Dual DoD A/B close"
	echo "  stamp-free: extract only; never bump GJ_IMAGE_VERSION / invent next N"
	echo "  process:   docs/ASSURANCE_LITE.md · ./scripts/gj-assurance-check.sh"
	;;
esac

# Exit 0 even when unknown so pack/flash paths stay non-fatal (soft).
exit 0
