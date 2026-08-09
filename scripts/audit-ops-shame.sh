#!/bin/sh
# SPDX-License-Identifier: MIT OR Apache-2.0
#
# GreenJade — Asm / ops hall-of-shame audit (docs/PURE_C_CONCURRENCY_AND_OPS.md).
#
# Informational by default (exit 0). Soft!=product; not Dual DoD close;
# not a product bar. Optional --fail-on-wbinvd for severe pattern only.
#
# Patterns (HALL_OF_SHAME):
#   wbinvd              — whole-cache inv; never for DMA publish
#   rdrand / rdseed     — report any (hot-loop spam risk)
#   vmovdqu / vmovaps   — report near mmio-ish paths if any
#   fxrstor             — report (never from MMIO)
#   clflush             — count/report files (informational; not fail)
#   lock / __atomic     — note: check natural alignment (split-lock risk)
#
# Usage:
#   ./scripts/audit-ops-shame.sh
#   ./scripts/audit-ops-shame.sh --fail-on-wbinvd
#   ./scripts/audit-ops-shame.sh --help
#
# Env:
#   GJ_ROOT  — repo root (default: parent of scripts/)
#
# greppable: HALL_OF_SHAME PURE_C_CONCURRENCY Soft!=product audit-ops-shame
set -eu
root="${GJ_ROOT:-}"
if [ -z "$root" ]; then
	root="$(CDPATH= cd -- "$(dirname "$0")/.." && pwd)"
fi
cd "$root"

fail_on_wbinvd=0
for arg in "$@"; do
	case "$arg" in
	--fail-on-wbinvd) fail_on_wbinvd=1 ;;
	-h|--help)
		sed -n '2,30p' "$0" | sed 's/^# \{0,1\}//'
		exit 0
		;;
	*)
		echo "audit-ops-shame: unknown option: $arg" >&2
		exit 2
		;;
	esac
done

# Scope: freestanding kernel + product UDX hosts. Skip huge third_party / musl.
# Prefer rg; fall back to grep -R.
has_rg=0
if command -v rg >/dev/null 2>&1; then
	has_rg=1
fi

# Source scope only (policy docs/script self-mentions are not findings).
src_paths="kernel user/drivers user/udx user/netstackd user/libgj"

findings=0
severe=0
clflush_files=0
clflush_lines=0

section() {
	printf '\n== %s ==\n' "$1"
}

# Drop policy negation noise for severe patterns ("no wbinvd"; not real insn use).
filter_wbinvd_noise() {
	grep -Ev '(no[[:space:]]+wbinvd|avoid[[:space:]].*wbinvd|never[[:space:]].*wbinvd|HALL_OF_SHAME)' \
		2>/dev/null || true
}

# Collect matching lines from source trees. Args: regex
collect_src() {
	_pat="$1"
	_out=""
	if [ "$has_rg" -eq 1 ]; then
		# shellcheck disable=SC2086
		_out=$(rg -n --no-heading -S -g '*.c' -g '*.h' -g '*.S' -g '*.s' -g '*.asm' \
			-g '!**/third_party/**' -g '!**/libcgj/**' -g '!**/*.o' -g '!**/*.d' \
			"$_pat" $src_paths 2>/dev/null || true)
	else
		for _p in $src_paths; do
			[ -e "$_p" ] || continue
			_chunk=$(grep -RIn --include='*.c' --include='*.h' --include='*.S' \
				--include='*.s' --include='*.asm' -E "$_pat" "$_p" 2>/dev/null || true)
			if [ -n "$_chunk" ]; then
				if [ -n "$_out" ]; then
					_out=$(printf '%s\n%s\n' "$_out" "$_chunk")
				else
					_out="$_chunk"
				fi
			fi
		done
	fi
	printf '%s' "$_out"
}

# Print search; optional second arg "severe" applies wbinvd noise filter + SEVERE tag.
search_print() {
	_label="$1"
	_pat="$2"
	_mode="${3:-}"
	section "$_label"
	_raw=$(collect_src "$_pat")
	if [ "$_mode" = "severe" ]; then
		_out=$(printf '%s\n' "$_raw" | filter_wbinvd_noise | sed '/^$/d')
	else
		_out=$(printf '%s\n' "$_raw" | sed '/^$/d')
	fi
	if [ -z "$_out" ]; then
		printf '  (none in kernel/user source)\n'
		return 0
	fi
	_n=$(printf '%s\n' "$_out" | wc -l | tr -d ' ')
	printf '%s\n' "$_out" | head -n 40
	if [ "$_n" -gt 40 ]; then
		printf '  ... (%s more lines)\n' "$((_n - 40))"
	fi
	if [ "$_mode" = "severe" ]; then
		printf '  total_lines=%s SEVERE\n' "$_n"
		severe=$((severe + _n))
	else
		printf '  total_lines=%s\n' "$_n"
	fi
	findings=$((findings + _n))
	return 0
}

printf 'audit-ops-shame: root=%s Soft!=product HALL_OF_SHAME (informational)\n' "$root"
printf 'audit-ops-shame: scope=%s\n' "$src_paths"
printf 'audit-ops-shame: tool=%s\n' "$([ "$has_rg" -eq 1 ] && echo rg || echo grep)"

# --- wbinvd (severe if real use; policy "no wbinvd" comments filtered) ---
search_print "wbinvd (severe — avoid entirely on hot/DMA paths)" \
	'\bwbinvd\b' severe

# --- rdrand / rdseed ---
search_print "rdrand / rdseed (report any; avoid hot loops)" \
	'\b(rdrand|rdseed|RDRAND|RDSEED)\b'

# --- vector moves (possible MMIO misuse) ---
search_print "vmovdqu / vmovaps (review if near MMIO/BAR paths)" \
	'\b(vmovdqu|vmovaps|VMOVDQU|VMOVAPS)\b'

# --- fxrstor ---
search_print "fxrstor (never from MMIO; report any)" \
	'\b(fxrstor|FXRSTOR|xrstor|XRSTOR)\b'

# --- clflush informational ---
section "clflush (informational — prefer narrow range at DMA publish)"
clflush_out=$(collect_src '\bclflush\b')
clflush_out=$(printf '%s\n' "$clflush_out" | sed '/^$/d')
if [ -z "$clflush_out" ]; then
	printf '  (none)\n'
	clflush_files=0
	clflush_lines=0
else
	clflush_lines=$(printf '%s\n' "$clflush_out" | wc -l | tr -d ' ')
	clflush_files=$(printf '%s\n' "$clflush_out" | sed 's/:.*//' | sort -u | wc -l | tr -d ' ')
	printf '%s\n' "$clflush_out" | head -n 30
	if [ "$clflush_lines" -gt 30 ]; then
		printf '  ... (%s more lines)\n' "$((clflush_lines - 30))"
	fi
	printf '  files=%s lines=%s (info only; expected in dma_buf / ring publish)\n' \
		"$clflush_files" "$clflush_lines"
	findings=$((findings + clflush_lines))
fi

# --- lock / atomic alignment note ---
section "lock / __atomic (note: verify natural alignment — split-lock risk)"
printf '  Policy: lock words and RMW targets must be naturally aligned.\n'
printf '  gj_spinlock is u32; prefer SPIN_ALIGNED when adjacent to hot data.\n'
printf '  Sample hot atomic / asm lock sites (capped):\n'
lock_sample=""
if [ "$has_rg" -eq 1 ]; then
	lock_sample=$(rg -n --no-heading -S -g '*.c' -g '*.h' -g '*.S' \
		-g '!**/third_party/**' -g '!**/libcgj/**' \
		'(__atomic_(load|store|compare|exchange|fetch)|__sync_|\"lock |asm.*\block\b)' \
		kernel/include/gj/spinlock.h kernel/mm kernel/sync kernel/cpu \
		kernel/ipc kernel/sched 2>/dev/null | head -n 25 || true)
else
	lock_sample=$(grep -RIn --include='*.c' --include='*.h' --include='*.S' -E \
		'(__atomic_(load|store|compare|exchange|fetch)|__sync_|\"lock )' \
		kernel/include/gj/spinlock.h kernel/mm kernel/sync kernel/cpu \
		kernel/ipc kernel/sched 2>/dev/null | head -n 25 || true)
fi
if [ -z "$lock_sample" ]; then
	printf '  (no sample matches in narrow paths)\n'
else
	printf '%s\n' "$lock_sample"
fi
printf '  Manual review: unaligned packed structs + locked RMW = split-lock.\n'

# --- summary ---
printf '\n== summary ==\n'
printf '  findings_lines≈%s severe_wbinvd_src=%s clflush_files=%s Soft!=product\n' \
	"$findings" "$severe" "${clflush_files:-0}"
printf '  house rules: docs/PURE_C_CONCURRENCY_AND_OPS.md\n'
printf '  greppable: HALL_OF_SHAME PURE_C_CONCURRENCY Soft!=product\n'

if [ "$fail_on_wbinvd" -eq 1 ] && [ "$severe" -gt 0 ]; then
	printf 'audit-ops-shame: FAIL (--fail-on-wbinvd and wbinvd in source)\n' >&2
	exit 1
fi

printf 'audit-ops-shame: done (informational exit 0 unless --fail-on-wbinvd)\n'
exit 0
