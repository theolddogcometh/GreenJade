#!/usr/bin/env bash
# SPDX-License-Identifier: MIT OR Apache-2.0
#
# Soft product PASS summary for a boot/smoke serial log.
# Greps greppable product markers (sshd, scsi_mid, multi-seg, HDA multi, M0,
# soak_tib, aarch64 bring-up, TRAP #UD count). Always exits 0 so it can run
# in agent / smoke / preflight paths without failing the caller.
#
# Usage:
#   ./scripts/gj-product-summary.sh <logfile>
#   ./scripts/run-qemu.sh build/greenjade.elf 2>&1 | tee /tmp/gj.log
#   ./scripts/gj-product-summary.sh /tmp/gj.log
#   ./scripts/gj-soak-large-ram.sh   # large-RAM path; then summary on its log
#   GJ_AARCH64_LOG=build/aarch64-smoke.log ./scripts/gj-product-summary.sh \
#     build/aarch64-smoke.log
#
# Contrast: scripts/gj-quick-keys.sh is a hard presence gate (exit 1 on miss).
# This script only reports; exit is always soft 0.
# Large-RAM soak runner (soft): scripts/gj-soak-large-ram.sh
# aarch64 bare metal: scripts/run-aarch64.sh
# aarch64 via podman: scripts/gj-aarch64-podman-smoke.sh
# continuum wire lines: scripts/gj-continuum-makefile-snippet.sh --max
# Steam bar3 media: scripts/steam-bar3-check.sh
#
# Markers (kernel / freestanding embeds / aarch64 scaffold):
#   M0 OK                         kernel/main.c or aarch64 kmain bring-up
#   sshd: live spawn PASS         product sshd.elf default-on :22
#   scsi_mid: live spawn PASS     product scsi_mid.elf
#   TCP multi-segment PASS        netstackd multi-seg (sshd / Top50 path)
#   hda: multi-stream mixer PASS  HDA multi-stream mixer
#   pmm: soak_tib PASS            large-RAM hierarchical freelist (GJ_MEM=768G)
#   aarch64: * PASS               aarch64 scaffold path (optional)
#   TRAP #UD                      count only (volume signal for agents)
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"

if [[ "${1:-}" == "-h" || "${1:-}" == "--help" || $# -lt 1 ]]; then
	echo "usage: $0 <logfile>" >&2
	# soft: help is not a hard failure for agents piping logs
	exit 0
fi

log=$1
echo "gj-product-summary: $log"

if [[ ! -f "$log" ]]; then
	echo "  MISS: log not found (soft)"
	echo "gj-product-summary: DONE (no log) soft-exit 0"
	exit 0
fi

# Binary-safe greps (serial logs may contain NULs from early firmware)
gqa() {
	grep -a -E "$@" "$log" 2>/dev/null || true
}
gqa_q() {
	grep -a -q -E "$1" "$log" 2>/dev/null
}

hit=0
miss=0

# report <label> <regex>
# Prints ok/MISS; does not change exit status.
report() {
	local _label=$1
	local _pat=$2
	if gqa_q "$_pat"; then
		echo "  ok:   $_label"
		hit=$((hit + 1))
	else
		echo "  MISS: $_label  (/${_pat}/)"
		miss=$((miss + 1))
	fi
}

echo "  --- x86 / product embeds ---"
# Product PASSes (presence only — soft inventory)
report "M0 OK"              'M0 OK'
report "sshd live PASS"     'sshd: live spawn PASS|sshd: live'
report "scsi_mid live PASS" 'scsi_mid: live spawn PASS|scsi_mid: live'
report "multi-seg PASS"     'TCP multi-segment PASS|multi-segment PASS|multi-seg'
report "HDA multi PASS"     'hda: multi-stream mixer PASS|multi-stream mixer PASS|hda: multi-stream'
# Large-RAM hierarchical soak (kernel/mm/pmm.c); typical only with GJ_MEM=768G.
# Miss is soft — small QEMU decks soft-SKIP soak_tib and never print PASS.
report "soak_tib PASS"      'pmm: soak_tib PASS|soak_tib PASS'

echo "  --- aarch64 scaffold (optional) ---"
# Only meaningful on aarch64 serial; miss is soft on x86 logs.
report "aarch64 exceptions" 'aarch64: exceptions PASS'
report "aarch64 cpu"        'aarch64: cpu PASS'
report "aarch64 GIC"        'aarch64: GIC PASS'
report "aarch64 timer"      'aarch64: timer PASS'
report "aarch64 pmm"        'aarch64: pmm PASS'
report "aarch64 mmu"        'aarch64: mmu PASS'
report "aarch64 svc"        'aarch64: svc PASS'
report "aarch64 virtio"     'aarch64: virtio-mmio PASS'
report "aarch64 mem probe"  'aarch64: mem probe PASS'

# TRAP #UD — count only (not a miss; agents care about volume)
ud_n=$(gqa 'TRAP[[:space:]]+#UD' | wc -l | tr -d ' ' || true)
# Guard empty / non-numeric
case "$ud_n" in
'' | *[!0-9]*) ud_n=0 ;;
esac
echo "  info: TRAP #UD count: $ud_n"

# Optional host media / continuum side panels (never fail)
echo "  --- host side panels (soft) ---"
if [[ -f "$ROOT/scripts/steam-bar3-check.sh" ]]; then
	# Capture first status line only (bash so +x not required)
	bar3_line=$(bash "$ROOT/scripts/steam-bar3-check.sh" 2>/dev/null | head -n1 || true)
	echo "  info: ${bar3_line:-steam-bar3-check: (no output)}"
else
	echo "  info: steam-bar3-check: (script missing)"
fi
if [[ -f "$ROOT/scripts/gj-continuum-makefile-snippet.sh" ]]; then
	mx_line=$(bash "$ROOT/scripts/gj-continuum-makefile-snippet.sh" --max 2>/dev/null || true)
	echo "  info: continuum ${mx_line:-makefile_max=(unknown)}"
else
	echo "  info: continuum makefile_max=(helper missing)"
fi

echo "gj-product-summary: hit=$hit miss=$miss ud=$ud_n soft-exit 0"
exit 0
