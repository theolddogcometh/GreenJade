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
# Env:
#   GJ_STEAM_TREE   host cache (default build/steam-tree)
#   GJ_STEAM_STAGE  staged tree root (default build/steam-stage)
#
# See also: docs/STEAM_HWTEST.md, scripts/gj-product-summary.sh (serial PASSes)
set -euo pipefail

root="$(CDPATH= cd -- "$(dirname "$0")/.." && pwd)"
cd "$root" || exit 0

tree="${GJ_STEAM_TREE:-build/steam-tree}"
stage="${GJ_STEAM_STAGE:-build/steam-stage}"

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

# Prefer explicit STATUS under common stage / media / rootfs layouts
for cand in \
	"$stage/steam/STATUS" \
	"$tree/steam/STATUS" \
	"build/rootfs/steam/STATUS" \
	"build/rootfs/opt/steam/STATUS" \
	"build/rootfs-steam-test/steam/STATUS"
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
echo "  see:    scripts/gj-product-summary.sh  scripts/gj-continuum-makefile-snippet.sh"
exit 0
