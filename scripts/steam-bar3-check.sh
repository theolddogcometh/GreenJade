#!/usr/bin/env bash
# SPDX-License-Identifier: MIT OR Apache-2.0
#
# Host-side soft check for Steam bar3 media readiness.
# Does NOT download Steam, start the client, or claim Deck Top 50 results.
# Always exits 0 (soft / exit-friendly) so it can run in smoke / preflight
# paths without failing the caller. Safe under set -e wrappers when invoked
# as a subprocess (this script never returns non-zero).
#
# Usage:
#   ./scripts/steam-bar3-check.sh
#   GJ_STEAM_TREE=build/steam-tree ./scripts/steam-bar3-check.sh
#
# Prints one line:
#   READY     — bootstrap client tree present (STATUS or launchers)
#   SKELETON  — staged placeholders only
#   MISSING   — no steam-tree / stage / STATUS found
#
# Soft only — never downloads Steam, never claims Deck Top 50 title results.
# Pair with serial inventory:
#   ./scripts/gj-product-summary.sh <serial-log>   # soft exit 0
#   ./scripts/gj-quick-keys.sh <serial-log>        # hard product keys
# Do not treat media READY as smoke-all green or bar3 client done.
#
# Soft deepen / honesty:
#   - Inventory tree/stage/rootfs file counts (capped)
#   - Soft STATUS consistency across layouts
#   - Soft launcher kind + bootstrap blob presence
#   - Soft MANIFEST / READY / STAGE_META stamps
#   - Always: bar3 client run OPEN; Top50 NOT-TRIED
#
# Wave 85 soft note (exclusive scripts high-water era):
#   Continuum soft high-water advancing toward M=22700 (CREATE-ONLY soft graph).
#   Soft deepen surfaces: retcrownangle / retshardangle (CREATE-ONLY soft names only).
#   Honest makefile_max is a Makefile scan — may still report 22600 until parent wires 22700.
#   Do not hardcode false makefile_max=22700. Product lamps remain 0.
#   multi_server=0 confine=0 bar3 OPEN lamps 0.
#   media READY ≠ continuum wire ≠ smoke-all PASS ≠ title PASS ≠ product complete.
#   Soft ≠ bar3; Top50 stays NOT-TRIED until real DUT title runs.
#
# Env:
#   GJ_STEAM_TREE   host cache (default build/steam-tree)
#   GJ_STEAM_STAGE  staged tree root (default build/steam-stage)
#   GJ_STEAM_ROOTFS rootfs steam path (default build/rootfs)
#
# See also: docs/STEAM_BAR3_STATUS.md, docs/STEAM_HWTEST.md
set -euo pipefail

root="$(CDPATH= cd -- "$(dirname "$0")/.." && pwd)"
cd "$root" || exit 0

tree="${GJ_STEAM_TREE:-build/steam-tree}"
stage="${GJ_STEAM_STAGE:-build/steam-stage}"
rootfs="${GJ_STEAM_ROOTFS:-build/rootfs}"

status=""
source=""

read_status() {
	local f=$1
	if [[ -f "$f" ]]; then
		# first non-empty token
		tr -d '\r' <"$f" | head -n1 | awk '{print $1}'
	fi
}

has_client_tree() {
	local base=$1
	[[ -e "$base/usr/bin/steam" || -L "$base/usr/bin/steam" ||
		-e "$base/bin/steam" ||
		-d "$base/usr/lib/steam" || -d "$base/usr" ]]
}

# Soft count of files under a tree (agent honesty; 0 on miss).
soft_file_count() {
	local d=$1
	if [[ ! -d "$d" ]]; then
		echo 0
		return 0
	fi
	# find may be slow on huge trees; cap via head. Soft on error.
	find "$d" -type f 2>/dev/null | head -n 5000 | wc -l | tr -d ' ' || echo 0
}

soft_launcher_kind() {
	local base=$1
	if [[ ! -d "$base" ]]; then
		echo "n/a"
		return 0
	fi
	if [[ -L "$base/usr/bin/steam" ]]; then
		echo "symlink→$(readlink "$base/usr/bin/steam" 2>/dev/null || echo ?)"
	elif [[ -f "$base/usr/bin/steam" ]]; then
		# soft: shebang peek (first 32 bytes printable)
		local head
		head=$(head -c 32 "$base/usr/bin/steam" 2>/dev/null | tr -cd '\11\12\15\40-\176' || true)
		if [[ "$head" == \#!* ]]; then
			echo "script"
		else
			echo "file"
		fi
	elif [[ -f "$base/bin/steam" ]]; then
		echo "bin/steam"
	elif [[ -f "$base/bin/steam.placeholder" ]]; then
		echo "placeholder"
	else
		echo "missing"
	fi
}

soft_bootstrap_blob() {
	local base=$1
	local b
	shopt -s nullglob 2>/dev/null || true
	for b in "$base/usr/lib/steam"/bootstraplinux_*.tar.xz \
		"$base/usr/lib/steam"/bootstraplinux_*.tar.gz; do
		if [[ -f "$b" ]]; then
			local sz
			sz=$(wc -c <"$b" 2>/dev/null | tr -d ' ' || echo 0)
			echo "$(basename "$b") (${sz}B)"
			return 0
		fi
	done
	echo "none"
}

soft_deb_cache() {
	local deb="$tree/download/steam.deb"
	if [[ -f "$deb" ]]; then
		local sz
		sz=$(wc -c <"$deb" 2>/dev/null | tr -d ' ' || echo 0)
		echo "present (${sz}B)"
	else
		echo "absent"
	fi
}

# Prefer explicit STATUS under common stage / media / rootfs layouts
# (dedupe by real path so rootfs env alias does not double-hit)
declare -a status_hits=()
declare -A status_seen=()
for cand in \
	"$stage/steam/STATUS" \
	"$tree/steam/STATUS" \
	"$rootfs/steam/STATUS" \
	"$rootfs/opt/steam/STATUS" \
	"build/rootfs/steam/STATUS" \
	"build/rootfs/opt/steam/STATUS" \
	"build/rootfs-steam-test/steam/STATUS" \
	"build/rootfs-steam-test/opt/steam/STATUS"
do
	if [[ -f "$cand" ]]; then
		_rp=$(readlink -f "$cand" 2>/dev/null || echo "$cand")
		if [[ -n "${status_seen[$_rp]:-}" ]]; then
			continue
		fi
		status_seen[$_rp]=1
		_st=$(read_status "$cand")
		status_hits+=("${cand}=${_st}")
		if [[ -z "$status" ]]; then
			status=$_st
			source=$cand
		fi
	fi
done

# Fallback: inspect host cache READY + launcher tree
if [[ -z "$status" ]]; then
	if has_client_tree "$tree/steam" || [[ -f "$tree/READY" ]]; then
		if has_client_tree "$tree/steam"; then
			status="READY"
			source="$tree/steam (launcher present)"
		else
			status="SKELETON"
			source="$tree (READY without launcher)"
		fi
	elif [[ -d "$stage/steam" ]]; then
		if has_client_tree "$stage/steam"; then
			status="READY"
			source="$stage/steam (launcher present)"
		else
			status="SKELETON"
			source="$stage/steam (no launcher)"
		fi
	else
		status="MISSING"
		source="none"
	fi
fi

# Normalize
case "$status" in
READY | ready | Ready)
	status="READY"
	;;
SKELETON | skeleton | Skeleton)
	status="SKELETON"
	;;
MISSING | missing | "")
	status="MISSING"
	;;
*)
	# Unknown STATUS token — treat as skeleton unless launchers exist
	if has_client_tree "$tree/steam" || has_client_tree "$stage/steam"; then
		status="READY"
	else
		status="SKELETON"
	fi
	;;
esac

echo "steam-bar3-check: $status"
echo "  source: $source"
echo "  tree:   $tree"
echo "  stage:  $stage"
echo "  rootfs: $rootfs"

# Soft inventory of media depth (never fails; honesty bounds for agents)
tree_n=$(soft_file_count "$tree")
stage_n=$(soft_file_count "$stage")
rootfs_steam_n=0
if [[ -d "$rootfs/steam" ]]; then
	rootfs_steam_n=$(soft_file_count "$rootfs/steam")
elif [[ -d "$rootfs/opt/steam" ]]; then
	rootfs_steam_n=$(soft_file_count "$rootfs/opt/steam")
fi
echo "  files:  tree≈$tree_n stage≈$stage_n rootfs_steam≈$rootfs_steam_n (capped sample; soft)"

# Soft STATUS multi-hit consistency (agent honesty when layouts diverge)
if [[ ${#status_hits[@]} -gt 0 ]]; then
	echo "  hits:   ${status_hits[*]}"
	_norm_set=""
	_diverge=0
	for h in "${status_hits[@]}"; do
		_v=${h##*=}
		case "$_v" in
		READY | ready | Ready) _v=READY ;;
		SKELETON | skeleton | Skeleton) _v=SKELETON ;;
		esac
		if [[ -z "$_norm_set" ]]; then
			_norm_set=$_v
		elif [[ "$_norm_set" != "$_v" ]]; then
			_diverge=1
		fi
	done
	if [[ "$_diverge" -eq 1 ]]; then
		echo "  warn:   STATUS tokens diverge across layouts (soft; inspect hits)"
	else
		echo "  agree:  STATUS tokens consistent across hits (soft)"
	fi
fi

# Soft launcher / blob probes
lk_tree=$(soft_launcher_kind "$tree/steam")
lk_stage=$(soft_launcher_kind "$stage/steam")
bb_tree=$(soft_bootstrap_blob "$tree/steam")
bb_stage=$(soft_bootstrap_blob "$stage/steam")
echo "  launch: tree=$lk_tree stage=$lk_stage"
echo "  blob:   tree=$bb_tree"
echo "  blob:   stage=$bb_stage"
echo "  deb:    $(soft_deb_cache)"

case "$status" in
READY)
	echo "  media:  bootstrap tree present (option 2/3 path)"
	echo "  open:   DUT launch + Deck Top 50 still NOT-TRIED (bar3 client run)"
	echo "  claim:  targeting only — media READY ≠ title PASS"
	# Optional manifest / READY stamp for agents
	if [[ -f "$tree/READY" ]]; then
		_ready=$(tr -d '\r' <"$tree/READY" 2>/dev/null | head -n1 | head -c 40 || true)
		echo "  stamp:  $tree/READY present${_ready:+ ($_ready)}"
	fi
	if [[ -f "$tree/MANIFEST.txt" ]]; then
		echo "  manif:  $tree/MANIFEST.txt present"
		# Soft key lines (url / sha / honesty)
		while IFS= read -r _line; do
			case "$_line" in
			url=* | deb_sha256=* | deb_bytes=* | honesty=* | launcher=* | bootstrap_blob=*)
				# truncate long lines softly
				_show=$(printf '%s' "$_line" | head -c 140)
				echo "  manif:  $_show"
				;;
			esac
		done <"$tree/MANIFEST.txt" 2>/dev/null || true
	fi
	if [[ -f "$stage/steam/STAGE_META.txt" ]]; then
		echo "  meta:   $stage/steam/STAGE_META.txt present"
		_ml=$(tr -d '\r' <"$stage/steam/STAGE_META.txt" 2>/dev/null | grep -E '^(status|honesty|files)' | head -n3 || true)
		if [[ -n "${_ml:-}" ]]; then
			while IFS= read -r _l; do
				[[ -n "$_l" ]] && echo "  meta:   $_l"
			done <<<"$_ml"
		fi
	fi
	if [[ -f "$stage/steam/HOST_PREP_META.txt" ]]; then
		echo "  meta:   $stage/steam/HOST_PREP_META.txt present (option 3 stamp)"
	fi
	if has_client_tree "$tree/steam"; then
		echo "  launch: tree/steam launcher paths present"
	fi
	if has_client_tree "$stage/steam"; then
		echo "  launch: stage/steam launcher paths present"
	fi
	if [[ -d "$rootfs/steam" ]] || [[ -d "$rootfs/opt/steam" ]]; then
		echo "  rootfs: steam dir present under $rootfs"
	fi
	;;
SKELETON)
	echo "  media:  placeholders only"
	echo "  open:   bar3 client run + Deck Top 50 still NOT-TRIED"
	echo "  next:   ./scripts/fetch-steam-bootstrap.sh && ./scripts/stage-steam-tree.sh"
	echo "  next:   # or: make steam-fetch && make steam-stage"
	;;
MISSING)
	echo "  media:  no steam-tree / stage found"
	echo "  open:   bar3 client run + Deck Top 50 still NOT-TRIED"
	echo "  next:   ./scripts/fetch-steam-bootstrap.sh && ./scripts/stage-steam-tree.sh"
	;;
esac

echo "  note:   soft check — exit 0 always; does not download or run Steam"
echo "  note:   media READY ≠ smoke-all PASS ≠ bar3 client / Top50"
echo "  note:   Wave 85 soft  continuum toward M=22700 ≠ bar3; makefile_max=scan truth (may be 22600); retcrownangle/retshardangle soft only"
echo "  note:   soft ≠ product complete; product lamps 0; Top50 NOT-TRIED × 50"
echo "  bar3:   OPEN (client launch + matrix fill still required)"
echo "  see:    docs/STEAM_BAR3_STATUS.md  docs/STEAM_HWTEST.md"
echo "  see:    scripts/gj-product-summary.sh  scripts/gj-quick-keys.sh"
exit 0
