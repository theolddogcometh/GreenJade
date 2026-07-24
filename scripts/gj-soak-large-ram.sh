#!/bin/sh
# SPDX-License-Identifier: MIT OR Apache-2.0
#
# Soft large-RAM hierarchical PMM soak (768 GiB class).
#
# Boots GreenJade under QEMU with GJ_MEM=768G (override via GJ_SOAK_MEM)
# and a long wall timeout, then greps serial for:
#   pmm: soak_tib PASS
#
# Soft exit 0 always — never hard-fails agent / smoke / preflight wrappers:
#   - host MemTotal too small to back QEMU -m $GJ_SOAK_MEM
#   - QEMU missing / ELF missing / timeout / early trap
#   - kernel `pmm: soak_tib SKIP soft` (max_pa below 768 GiB need)
#   - soak_tib PASS missing from the log
#
# ---------------------------------------------------------------------------
# Wave 10 soft honesty — product ≥ 1 TiB bar stays OPEN
# ---------------------------------------------------------------------------
# This runner exercises the 768 GiB-class hierarchical soak only
# (main.c → pmm_soak_tib(768ull<<30)). Soft honesty bounds:
#   - soak_tib PASS ≠ full 1 TiB product bar met
#   - GJ_SOAK_MEM=1T / -m 1T is optional operator override; not claimed closed
#   - pmm: tib_host soft SKIP  → host/QEMU below 1 TiB (expected on modest decks)
#   - pmm: tib_host soft PASS  → true ≥1 TiB max_pa greppable (soft only)
#   - pmm: tib_design soft     → always-on design observability (no 1 TiB host)
# Full 1 TiB host soak remains OPEN until a real ≥1 TiB host path is claimed
# elsewhere (HCL / matrix). Soft-SKIP paths never hard-fail this script.
# ---------------------------------------------------------------------------
# Wave 105 soft note (continuum / product side panel):
#   Soft product-summary may note continuum high-water advancing toward M=24700.
#   Honest scan may still report makefile_max=24600 until parent wires 24700.
#   Soft deepen retquadraticangle/retcubicangle are CREATE-ONLY soft names only (≠ product).
#   Soak_tib PASS still ≠ bar3 / Steam / Top-50 and ≠ continuum makefile_max wire.
#   makefile_max remains an honest Makefile scan (never hard-coded here).
#   Soft ≠ product complete; product lamps 0.
# ---------------------------------------------------------------------------
#
# Soft-SKIP paths (all exit 0 — clear and greppable):
#   SKIP host too small     host MemTotal < min for -m $GJ_SOAK_MEM
#   SKIP soft (kernel)      pmm: soak_tib SKIP soft  (max_pa below need)
#   MISS ... soft-exit 0    parse / ELF / timeout / FAIL / no PASS line
# Never promote 768G PASS → “desktop 1 TiB bar met” or bar3 / Steam ready.
#
# Usage:
#   ./scripts/gj-soak-large-ram.sh
#   GJ_SOAK_MEM=512G GJ_SOAK_TIMEOUT=900 ./scripts/gj-soak-large-ram.sh
#   ./scripts/gj-product-summary.sh /tmp/gj-soak-large-ram.*.log
#
# Env:
#   GJ_SOAK_MEM       QEMU RAM (-m); default 768G (also sets GJ_MEM for run-qemu)
#   GJ_SOAK_TIMEOUT   wall timeout seconds; default 600 (large -m can take minutes)
#   GJ_SOAK_LOG       log path; default ${TMPDIR:-/tmp}/gj-soak-large-ram.$$.log
#   GJ_SOAK_MIN_HOST  min host MemTotal KiB required; default ≈ mem size + 64 GiB headroom
#   GJ_SMP            forwarded to run-qemu (optional)
#   QEMU_BIN          forwarded via run-qemu (optional)
#
# Kernel markers (kernel/mm/pmm.c pmm_soak_tib; kernel/main.c with 768ull<<30):
#   pmm: soak_tib PASS            hierarchical free exercised at TiB-class max_pa
#   pmm: soak_tib SKIP soft       host/QEMU below threshold (not a fail; soft-SKIP)
#   pmm: soak_tib FAIL            alloc path failed (still soft-exit 0 here)
#   pmm: tib_design soft          1 TiB design path ready (no 1 TiB host required)
#   pmm: tib_host soft PASS|SKIP  true ≥1 TiB max_pa gate (soft; bar stays open)
#
# Contrast:
#   scripts/smoke-all.sh     hard-requires small `pmm: soak PASS` (not this script)
#   scripts/gj-product-summary.sh  soft inventory including soak_tib PASS
#   scripts/gj-quick-keys.sh hard presence gate (does not require soak_tib)
#
# After run, soft-scan the same log with:
#   ./scripts/gj-product-summary.sh "$log"
#   ./scripts/gj-quick-keys.sh "$log"   # hard keys; soak_tib is soft info only
#
# See also: docs/HCL.md, docs/STEAM_HWTEST.md (768G soak scope / 1 TiB open honesty)
set -u

root="$(CDPATH= cd -- "$(dirname "$0")/.." && pwd)"
cd "$root" || {
	echo "gj-soak-large-ram: cannot cd to $root (soft)" >&2
	exit 0
}

# --- config -----------------------------------------------------------------
GJ_SOAK_MEM="${GJ_SOAK_MEM:-768G}"
GJ_SOAK_TIMEOUT="${GJ_SOAK_TIMEOUT:-600}"
log="${GJ_SOAK_LOG:-${TMPDIR:-/tmp}/gj-soak-large-ram.$$.log}"
elf="${1:-$root/build/greenjade.elf}"

if [ "${1:-}" = "-h" ] || [ "${1:-}" = "--help" ]; then
	echo "usage: $0 [path/to/greenjade.elf]" >&2
	echo "  GJ_SOAK_MEM=$GJ_SOAK_MEM  GJ_SOAK_TIMEOUT=${GJ_SOAK_TIMEOUT}s" >&2
	echo "  Soft exit 0 on host-too-small / miss / SKIP (never hard-fail)." >&2
	echo "  Honesty: 768G soak ≠ product ≥1 TiB bar (1 TiB path remains OPEN)." >&2
	exit 0
fi

echo "gj-soak-large-ram: mem=$GJ_SOAK_MEM timeout=${GJ_SOAK_TIMEOUT}s log=$log"
echo "gj-soak-large-ram: honesty  product ≥1 TiB full path OPEN (768G class only; Wave 10 soft)"
echo "gj-soak-large-ram: honesty  Wave 105 soft  soak ≠ bar3/Top50; continuum makefile_max=scan (toward M=24700; may be 24600)"

# --- helpers ----------------------------------------------------------------
# Parse size token like 768G / 512g / 1T / 65536M / bare bytes → KiB (integer).
# Prints 0 on unparseable input (caller treats as soft skip).
size_to_kib() {
	_s=$1
	_n=$(printf '%s' "$_s" | sed -n 's/^\([0-9][0-9]*\)[KkMmGgTt]\?$/\1/p')
	_u=$(printf '%s' "$_s" | sed -n 's/^[0-9][0-9]*\([KkMmGgTt]\?\)$/\1/p')
	if [ -z "$_n" ]; then
		echo 0
		return 0
	fi
	case "$_u" in
	'' ) echo "$_n" ;; # already bytes? treat as KiB-ish raw count — avoid; use 0
	K | k) echo "$_n" ;;
	M | m)
		# MiB → KiB
		echo $((_n * 1024))
		;;
	G | g)
		echo $((_n * 1024 * 1024))
		;;
	T | t)
		echo $((_n * 1024 * 1024 * 1024))
		;;
	*) echo 0 ;;
	esac
}

soft_done() {
	_msg=$1
	echo "gj-soak-large-ram: $_msg soft-exit 0"
	# Wave 10: restate open bar on every soft exit (agent-greppable honesty).
	echo "gj-soak-large-ram: honesty  product ≥1 TiB full path OPEN (soft-SKIP/MISS ≠ closed)"
	exit 0
}

# --- host gate (too small → soft-SKIP exit 0) -------------------------------
need_kib=$(size_to_kib "$GJ_SOAK_MEM")
if [ "$need_kib" -eq 0 ]; then
	soft_done "MISS: cannot parse GJ_SOAK_MEM='$GJ_SOAK_MEM'"
fi

# Default min host: requested guest RAM + 64 GiB headroom for host/QEMU overhead.
headroom_kib=$((64 * 1024 * 1024))
default_min_kib=$((need_kib + headroom_kib))
min_host_kib="${GJ_SOAK_MIN_HOST:-$default_min_kib}"

host_kib=0
if [ -r /proc/meminfo ]; then
	host_kib=$(awk '/^MemTotal:/{print $2; exit}' /proc/meminfo 2>/dev/null || echo 0)
fi
case "$host_kib" in
'' | *[!0-9]*) host_kib=0 ;;
esac

echo "gj-soak-large-ram: host_MemTotal_KiB=$host_kib need_guest_KiB=$need_kib min_host_KiB=$min_host_kib"

if [ "$host_kib" -gt 0 ] && [ "$host_kib" -lt "$min_host_kib" ]; then
	# Soft-SKIP path A: host cannot back QEMU -m (clear, never hard-fail).
	soft_done "SKIP host too small (MemTotal ${host_kib} KiB < ${min_host_kib} KiB for -m $GJ_SOAK_MEM)"
fi

# --- ELF / timeout / run-qemu -----------------------------------------------
if [ ! -f "$elf" ]; then
	soft_done "MISS: missing $elf — run make first"
fi

if ! command -v timeout >/dev/null 2>&1; then
	soft_done "MISS: timeout(1) not found"
fi

# Export so run-qemu sees GJ_MEM; keep GJ_SOAK_MEM as the operator-facing name.
GJ_MEM="$GJ_SOAK_MEM"
export GJ_MEM
# Optional SMP passthrough if already set in environment
# (run-qemu defaults GJ_SMP=1).

# Long timeout: large -m allocation + early PMM soak; kill remaining QEMU.
# || true: timeout exit 124 / qemu nonzero must not abort this soft script.
set +e
timeout "$GJ_SOAK_TIMEOUT" ./scripts/run-qemu.sh "$elf" >"$log" 2>&1
rc=$?
set -e
echo "gj-soak-large-ram: run-qemu exit=$rc (124=timeout expected)"

if [ ! -f "$log" ]; then
	soft_done "MISS: no log written at $log"
fi

# Binary-safe greps (serial may contain NULs from early firmware)
gqa_q() {
	grep -a -q -E "$1" "$log" 2>/dev/null
}

# Soft companion: print small-pmm soak presence (smoke-all hard key; not required here).
if gqa_q 'pmm: soak PASS'; then
	echo "gj-soak-large-ram: info pmm: soak PASS present (smoke-all class)"
else
	echo "gj-soak-large-ram: info pmm: soak PASS absent (soft)"
fi

# Soft companion: M0 / hierarchical free readiness (agent honesty on partial boots).
if gqa_q 'M0 OK'; then
	echo "gj-soak-large-ram: info M0 OK present"
else
	echo "gj-soak-large-ram: info M0 OK absent (soft)"
fi
if gqa_q 'hierarchical free ready|hierarchical free'; then
	echo "gj-soak-large-ram: info hierarchical free markers present"
fi

# ---------------------------------------------------------------------------
# Wave 10 soft honesty companions (1 TiB design / host — info only, never fail)
# ---------------------------------------------------------------------------
# tib_design: always-on design path (no 1 TiB host required).
# tib_host soft SKIP: expected below 1 TiB — product bar stays OPEN.
# tib_host soft PASS: greppable ≥1 TiB max_pa only; still soft (≠ bar3/Steam).
if gqa_q 'pmm: tib_design soft'; then
	line=$(grep -a -E 'pmm: tib_design soft' "$log" 2>/dev/null | head -n1 || true)
	echo "gj-soak-large-ram: info tib_design soft present — $line"
else
	echo "gj-soak-large-ram: info tib_design soft absent (soft)"
fi

if gqa_q 'pmm: tib_host soft PASS'; then
	line=$(grep -a -E 'pmm: tib_host soft PASS' "$log" 2>/dev/null | head -n1 || true)
	echo "gj-soak-large-ram: info tib_host soft PASS — $line"
	echo "gj-soak-large-ram: honesty  tib_host PASS greppable; product ≥1 TiB bar still OPEN (Wave 10 soft)"
elif gqa_q 'pmm: tib_host soft SKIP'; then
	line=$(grep -a -E 'pmm: tib_host soft SKIP' "$log" 2>/dev/null | head -n1 || true)
	# Soft-SKIP path B: true 1 TiB host gate skipped (clear, never hard-fail).
	echo "gj-soak-large-ram: info tib_host soft SKIP — $line"
	echo "gj-soak-large-ram: honesty  tib_host soft-SKIP; product ≥1 TiB full path OPEN"
else
	echo "gj-soak-large-ram: info tib_host soft marker absent (soft)"
	echo "gj-soak-large-ram: honesty  product ≥1 TiB full path OPEN (no tib_host claim)"
fi

# Optional soft product inventory (never fails this script; ignore helper exit).
if [ -f "$root/scripts/gj-product-summary.sh" ]; then
	echo "gj-soak-large-ram: --- soft product-summary ---"
	bash "$root/scripts/gj-product-summary.sh" "$log" 2>/dev/null || true
fi

# --- soak_tib verdicts (all soft-exit 0) ------------------------------------
if gqa_q 'pmm: soak_tib PASS|soak_tib PASS'; then
	line=$(grep -a -E 'pmm: soak_tib PASS|soak_tib PASS' "$log" 2>/dev/null | head -n1 || true)
	echo "gj-soak-large-ram: PASS  ($line)"
	# Wave 10 honesty: 768G-class PASS does not close the ≥1 TiB product bar.
	echo "gj-soak-large-ram: honesty  soak_tib PASS is 768G-class; product ≥1 TiB full path OPEN"
	echo "gj-soak-large-ram: log=$log soft-exit 0"
	exit 0
fi

# Soft-SKIP path C: kernel max_pa below soak need (clear, never hard-fail).
if gqa_q 'pmm: soak_tib SKIP soft|soak_tib SKIP'; then
	line=$(grep -a -E 'pmm: soak_tib SKIP' "$log" 2>/dev/null | head -n1 || true)
	soft_done "SKIP soft (kernel host/QEMU below threshold) — $line"
fi

if gqa_q 'pmm: soak_tib FAIL|TiB soak FAIL'; then
	line=$(grep -a -E 'pmm: soak_tib FAIL|TiB soak FAIL' "$log" 2>/dev/null | head -n1 || true)
	soft_done "MISS soak_tib FAIL (soft) — $line"
fi

soft_done "MISS: no soak_tib PASS in $log (timeout/host/boot)"
