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
# Env:
#   GJ_STEAM_TREE   host cache (default build/steam-tree)
#   GJ_STEAM_STAGE  staged tree root (default build/steam-stage)
#   GJ_STEAM_ROOTFS rootfs steam path (default build/rootfs)
#
# See also: docs/STEAM_HWTEST.md, scripts/gj-product-summary.sh (serial PASSes)
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
	[[ -e "$base/usr/bin/steam" || -e "$base/bin/steam" ||
		-d "$base/usr/lib/steam" || -d "$base/usr" ]]
}

# Soft count of non-empty files under a tree (agent honesty; 0 on miss).
soft_file_count() {
	local d=$1
	if [[ ! -d "$d" ]]; then
		echo 0
		return 0
	fi
	# find may be slow on huge trees; cap via head. Soft on error.
	find "$d" -type f 2>/dev/null | head -n 5000 | wc -l | tr -d ' ' || echo 0
}

# Prefer explicit STATUS under common stage / media / rootfs layouts
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
		status=$(read_status "$cand")
		source=$cand
		break
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
echo "  files:  tree≈$tree_n stage≈$stage_n (capped sample; soft)"

case "$status" in
READY)
	echo "  media:  bootstrap tree present (option 2/3 path)"
	echo "  open:   DUT launch + Deck Top 50 still NOT-TRIED (bar3 client run)"
	# Optional manifest / READY stamp for agents
	if [[ -f "$tree/READY" ]]; then
		echo "  stamp:  $tree/READY present"
	fi
	if [[ -f "$tree/MANIFEST.txt" ]]; then
		echo "  manif:  $tree/MANIFEST.txt present"
		# First non-empty line of manifest (soft; may be long — truncate)
		_ml=$(tr -d '\r' <"$tree/MANIFEST.txt" 2>/dev/null | head -n1 | head -c 120 || true)
		if [[ -n "${_ml:-}" ]]; then
			echo "  manif0: $_ml"
		fi
	fi
	# Soft launcher probes
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
	echo "  next:   make steam-fetch && make steam-stage   # or GJ_SKIP_FETCH=1 if deb cached"
	;;
MISSING)
	echo "  media:  no steam-tree / stage found"
	echo "  next:   make steam-fetch && make steam-stage"
	;;
esac

echo "  note:   soft check — exit 0 always; does not download or run Steam"
echo "  note:   media READY ≠ smoke-all PASS ≠ bar3 client / Top50"
echo "  see:    scripts/gj-product-summary.sh  scripts/gj-quick-keys.sh"
echo "  see:    scripts/gj-continuum-makefile-snippet.sh  docs/STEAM_HWTEST.md"
exit 0
