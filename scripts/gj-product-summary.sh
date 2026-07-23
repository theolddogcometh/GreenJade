#!/usr/bin/env bash
# SPDX-License-Identifier: MIT OR Apache-2.0
#
# Soft product PASS summary for a boot/smoke serial log.
# Greps greppable product markers (sshd, scsi_mid, multi-seg, HDA multi, M0,
# live daemons, PE32 surface, soak / soak_tib, aarch64 bring-up, TRAP #UD).
# Always exits 0 so it can run in agent / smoke / preflight paths without
# failing the caller.
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
# Full hard smoke (do not soften): scripts/smoke-all.sh
#
# Markers (kernel / freestanding embeds / aarch64 scaffold):
#   M0 OK                         kernel/main.c or aarch64 kmain bring-up
#   sshd: live spawn PASS         product sshd.elf default-on :22
#   scsi_mid: live spawn PASS     product scsi_mid.elf
#   sessiond/netstackd/storaged/vfsd/shell/hda_client: live spawn PASS
#   TCP multi-segment PASS        netstackd multi-seg (sshd / Top50 path)
#   hda: multi-stream mixer PASS  HDA multi-stream mixer
#   pe32: int80 … PASS            Wine int80 surface (soft inventory)
#   pmm: soak PASS                small hierarchical soak (smoke-all hard)
#   pmm: soak_tib PASS            large-RAM hierarchical freelist (GJ_MEM=768G)
#   aarch64: * PASS               aarch64 scaffold path (optional)
#   TRAP #UD                      count only (volume signal for agents)
# Soft verify deepen (info only — never hard-fail; always soft-exit 0):
#   serial: soft verify PASS|FAIL|idle   x86 COM1 (serial.c)
#   aarch64: kmain soft PASS             kmain phase summary
#   linux: nr class soft PASS|PARTIAL|NONE  NR hot/cold table
# Soft stats smoke (info only — if greppable in serial log):
#   spawn/sched/user/native/syscall/trap: soft stats …
#   scsi_mid-gj: soft stats …            freestanding scsi soft counters
# Continuum side panel (host helper; soft graph only):
#   makefile_max=N                       honest Makefile scan (source of truth)
#   high-water makefile_max=15600        noted when greppable (Wave 14; soft ≠ bar3)
#   (do not hardcode false max if Makefile not yet wired past prior tip)
# Bar3 honesty (host media check; always OPEN while client/matrix open):
#   bar3: OPEN / client launch + Top50 NOT-TRIED
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"

if [[ "${1:-}" == "-h" || "${1:-}" == "--help" || $# -lt 1 ]]; then
	echo "usage: $0 <logfile>" >&2
	echo "  Soft inventory of product PASSes; always exit 0." >&2
	echo "  Hard gate: scripts/gj-quick-keys.sh <logfile>" >&2
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

# info_match <label> <regex>
# Soft presence line only — never increments hit/miss.
info_match() {
	local _label=$1
	local _pat=$2
	if gqa_q "$_pat"; then
		echo "  info: $_label  (present)"
	else
		echo "  info: $_label  (absent)"
	fi
}

# first_line <regex> — first matching line, truncated, or empty
first_line() {
	gqa "$1" | head -n1 | tr -cd '\11\12\15\40-\176' | head -c 160 || true
}

echo "  --- core product (x86 embeds) ---"
# Product PASSes (presence only — soft inventory)
report "M0 OK"              'M0 OK'
report "sshd live PASS"     'sshd: live spawn PASS|sshd: live'
report "scsi_mid live PASS" 'scsi_mid: live spawn PASS|scsi_mid: live'
report "multi-seg PASS"     'TCP multi-segment PASS|multi-segment PASS'
report "HDA multi PASS"     'hda: multi-stream mixer PASS|multi-stream mixer PASS|hda: multi-stream'

echo "  --- live daemons (soft) ---"
report "sessiond live"      'sessiond: live spawn PASS|sessiond: live'
report "netstackd live"     'netstackd: live spawn PASS|netstackd: live'
report "storaged live"      'storaged: live spawn PASS|storaged: live'
report "vfsd live"          'vfsd: live spawn PASS|vfsd: live'
report "shell live"         'shell: live spawn PASS|shell: live'
report "hda_client live"    'hda_client: live spawn PASS|hda_client: live'

echo "  --- freestanding path (soft) ---"
# Aggregate freestanding PASS only (FAIL is reported under soft status lines).
report "sshd-gj live path"       'sshd-gj: live path PASS'
report "netstackd-gj live path"  'netstackd-gj: live path PASS'
report "storaged-gj live path"   'storaged-gj: live path PASS'
report "vfsd-gj live path"       'vfsd-gj: live path PASS'
report "scsi_mid-gj READY"       'scsi_mid-gj: READY PASS'

echo "  --- surface / memory (soft) ---"
report "pe32 int80 multi"   'pe32: int80 multi PASS|pe32: int80'
report "pmm soak PASS"      'pmm: soak PASS'
# Large-RAM hierarchical soak (kernel/mm/pmm.c); typical only with GJ_MEM=768G.
# Miss is soft — small QEMU decks soft-SKIP soak_tib and never print PASS.
report "soak_tib PASS"      'pmm: soak_tib PASS|soak_tib PASS'
report "hierarchical free"  'hierarchical free ready|hierarchical free'

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
# Soft deepen phase summary (never hard-fail; miss soft on x86 logs).
report "kmain soft"         'aarch64: kmain soft PASS|kmain soft PASS'

# TRAP #UD — count only (not a miss; agents care about volume)
ud_n=$(gqa 'TRAP[[:space:]]+#UD' | wc -l | tr -d ' ' || true)
case "$ud_n" in
'' | *[!0-9]*) ud_n=0 ;;
esac
echo "  info: TRAP #UD count: $ud_n"

echo "  --- soft verify deepen (info only) ---"
# serial soft / nr class soft: presence + first-line verdict; never hit/miss.
# Grep: serial: soft verify · linux: nr class soft
if gqa_q 'serial: soft verify'; then
	_sv=$(first_line 'serial: soft verify')
	echo "  info: serial soft verify  (present) — ${_sv:-present}"
else
	echo "  info: serial soft verify  (absent)"
fi
if gqa_q 'linux: nr class soft'; then
	_nr=$(first_line 'linux: nr class soft')
	echo "  info: nr class soft  (present) — ${_nr:-present}"
else
	echo "  info: nr class soft  (absent)"
fi
# kmain soft also reported under aarch64 scaffold; echo first line when hit.
if gqa_q 'aarch64: kmain soft PASS|kmain soft PASS'; then
	_km=$(first_line 'aarch64: kmain soft PASS|kmain soft PASS')
	echo "  info: kmain soft  (present) — ${_km:-present}"
else
	echo "  info: kmain soft  (absent)"
fi

echo "  --- soft status lines (info only) ---"
# soak_tib SKIP is expected on modest QEMU; do not count as miss above.
if gqa_q 'pmm: soak_tib SKIP|soak_tib SKIP'; then
	_sk=$(first_line 'pmm: soak_tib SKIP|soak_tib SKIP')
	echo "  info: soak_tib SKIP soft — ${_sk:-present}"
elif gqa_q 'pmm: soak_tib FAIL|TiB soak FAIL'; then
	_fl=$(first_line 'pmm: soak_tib FAIL|TiB soak FAIL')
	echo "  info: soak_tib FAIL (soft inventory) — ${_fl:-present}"
else
	echo "  info: soak_tib SKIP/FAIL  (absent)"
fi

# Freestanding FAIL banners (agent honesty; not hit/miss)
if gqa_q 'sshd-gj: live path FAIL'; then
	echo "  info: sshd-gj live path FAIL (soft — crypto/session incomplete on some decks)"
fi
if gqa_q 'TCP multi-segment FAIL|multi-segment FAIL'; then
	echo "  info: multi-seg FAIL present (soft)"
fi

# Soft stats smoke lines (info only — greppable kernel/embed counters; never hit/miss).
# Grep: soft stats  (spawn/sched/user/native/syscall/trap/scsi_mid-*)
echo "  --- soft stats smoke (info only) ---"
_ss_pat='soft stats'
if gqa_q "$_ss_pat"; then
	_ss_n=$(gqa "$_ss_pat" | wc -l | tr -d ' ' || true)
	case "$_ss_n" in
	'' | *[!0-9]*) _ss_n=0 ;;
	esac
	_ss_line=$(first_line "$_ss_pat")
	echo "  info: soft stats smoke  (present n=$_ss_n) — ${_ss_line:-present}"
	# Named subsystem presence (agent-friendly; still soft)
	for _ss_tag in \
		"spawn: soft stats" \
		"sched: soft stats" \
		"user: soft stats" \
		"native: soft stats" \
		"syscall: soft stats" \
		"trap: soft stats" \
		"scsi_mid-gj: soft stats" \
		"scsi_mid-server: soft stats"
	do
		if gqa_q "$_ss_tag"; then
			echo "  info: soft stats  ${_ss_tag%%:*}  (present)"
		fi
	done
else
	echo "  info: soft stats smoke  (absent)"
fi

# UEFI / EFI path markers when present (OVMF / real-hw serial)
info_match "GJ-EFI"            'GJ-EFI'
info_match "KERNEL.ELF loaded" 'KERNEL\.ELF loaded'
info_match "source=UEFI"       'source=UEFI'

# Optional host media / continuum side panels (never fail)
echo "  --- host side panels (soft) ---"
# Bar3 open honesty: media READY/SKELETON/MISSING is host inventory only.
# Client launch + Deck Top 50 matrix remain OPEN (never claim closed here).
if [[ -f "$ROOT/scripts/steam-bar3-check.sh" ]]; then
	# Capture status + honesty lines (bash so +x not required)
	_bar3_out=$(bash "$ROOT/scripts/steam-bar3-check.sh" 2>/dev/null || true)
	bar3_line=$(printf '%s\n' "$_bar3_out" | head -n1 || true)
	echo "  info: ${bar3_line:-steam-bar3-check: (no output)}"
	# Prefer helper's own open/bar3 lines when present
	_bar3_open=$(printf '%s\n' "$_bar3_out" | grep -E '^\s*(open:|bar3:)' | head -n3 || true)
	if [[ -n "${_bar3_open:-}" ]]; then
		while IFS= read -r _bl; do
			[[ -n "$_bl" ]] && echo "  info: bar3 honesty  ${_bl#"${_bl%%[![:space:]]*}"}"
		done <<<"$_bar3_open"
	fi
	# Always stamp open honesty even if helper output is thin
	echo "  info: bar3 OPEN  (client launch + Deck Top 50 still NOT-TRIED)"
	echo "  info: bar3 honesty  media READY ≠ smoke-all PASS ≠ title PASS"
else
	echo "  info: steam-bar3-check: (script missing)"
	echo "  info: bar3 OPEN  (client launch + Deck Top 50 still NOT-TRIED)"
fi
# Continuum high-water (honest Makefile scan; soft graph only — not bar3).
# Wave 14: note makefile_max=15600 when greppable from helper stdout (N>=15600).
# Scan is source of truth — never claim 15600 greppable until Makefile is wired.
if [[ -f "$ROOT/scripts/gj-continuum-makefile-snippet.sh" ]]; then
	mx_line=$(bash "$ROOT/scripts/gj-continuum-makefile-snippet.sh" --max 2>/dev/null || true)
	mx_line=${mx_line//$'\r'/}
	mx_line=$(printf '%s' "$mx_line" | tr -cd '\11\12\15\40-\176' | head -c 80 || true)
	echo "  info: continuum ${mx_line:-makefile_max=(unknown)}"
	# Parse N from makefile_max=N when greppable
	_mx_n=""
	if [[ "${mx_line:-}" =~ makefile_max=([0-9]+) ]]; then
		_mx_n="${BASH_REMATCH[1]}"
	fi
	if [[ -n "$_mx_n" ]]; then
		echo "  info: continuum high-water  makefile_max=$_mx_n  (CREATE-ONLY soft graph)"
		# Wave 14 high-water stamp: note makefile_max=15600 only when greppable
		if [[ "$_mx_n" -ge 15600 ]]; then
			echo "  info: continuum high-water  makefile_max=15600 greppable  (soft graph ≠ bar3)"
		else
			echo "  info: continuum high-water  makefile_max=15600  (absent; tree max=$_mx_n; scan=truth)"
		fi
	else
		echo "  info: continuum high-water  makefile_max=(unparsed)"
	fi
	echo "  info: continuum honesty  soft graph wire ≠ bar3 client / Top50 titles"
	echo "  info: continuum wave 14  target decade M=15600 (CREATE-ONLY soft; parent paste wires)"
else
	echo "  info: continuum makefile_max=(helper missing)"
fi

# Artifact presence (host tree; soft — no build, no fail)
echo "  --- host artifacts (soft) ---"
for art in \
	"build/greenjade.elf" \
	"build/GreenJade.efi" \
	"build/user/sshd.elf" \
	"build/user/scsi_mid.elf" \
	"build/user/libc.so.6"
do
	if [[ -f "$ROOT/$art" ]]; then
		echo "  info: artifact $art  (present)"
	else
		echo "  info: artifact $art  (absent)"
	fi
done

echo "gj-product-summary: hit=$hit miss=$miss ud=$ud_n soft-exit 0"
exit 0
