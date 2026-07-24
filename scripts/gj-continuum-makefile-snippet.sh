#!/usr/bin/env bash
# SPDX-License-Identifier: MIT OR Apache-2.0
#
# Continuum wire helper — print next CGJ_SRCS Makefile lines for graph_batchN.c
# in a closed range [FROM, TO]. Does not edit the Makefile; paste output into
# CGJ_SRCS (before the trailing non-batch sources such as rand48.c).
#
# Soft continuum role (honest scope)
# ----------------------------------
# GreenJade grows the clean-room libcgj *graph continuum* as CREATE-ONLY
# freestanding pure-C TUs (user/libcgj/src/graph_batchN.c). A continuum
# *decade* is a closed batch range ending at a milestone M (e.g. M=17200
# → exclusive wave 17151–17200). Soft host probes
# (user/libcgj/tests/cgj_soft_milestone_M.c) KAT the decade marker lamps
# only; identity/fill bands are structural unique surfaces.
#
# This script is the **parent paste half** of that workflow:
#
#   1. CREATE-ONLY sources land first (generators / exclusive agents).
#      Example generator for the 17200 wave (Wave 30 exclusive continuum track):
#        user/libcgj/src/_gen_milestone_17200.py
#        python3 user/libcgj/src/_gen_milestone_17200.py --dry-run
#      (prior Wave 29: _gen_milestone_17100.py / M=17100)
#   2. Parent wires CGJ_SRCS by pasting this helper's stdout into the
#      Makefile (this script never writes the Makefile itself).
#   3. Rebuild libcgj; optional soft milestone host probes.
#
# makefile_max honesty
# --------------------
# ``makefile_max`` is the **numeric maximum** of graph_batchN.c basenames
# already referenced in the Makefile (grep scan). It is not hard-coded,
# not a Steam readiness claim, and not advanced by this helper alone.
# The scan has **no artificial cap** — it correctly reports through
# graph_batch17200.c (and beyond) once those basenames are wired.
#
# Wave 30 soft note (17200-era): the exclusive target decade is M=17200
# (CREATE-ONLY 17151–17200; optional extension 17101–17150). Until parent
# pastes those basenames into CGJ_SRCS, ``--max`` may still report the
# prior high-water (e.g. makefile_max=17100). **Scan is the source of
# truth** — do not hardcode a false makefile_max here or in side panels.
# Soft high-water may be described as **advancing toward 17200** while the
# honest scan remains at the prior tip.
#
# Verify anytime (stdout is greppable by product-summary side panels):
#
#   ./scripts/gj-continuum-makefile-snippet.sh --max
#   # → makefile_max=N   (honest scan; greppable when N reaches the era)
#
# Do **not** document or assert a higher max than the Makefile scan
# reports. Soft graph growth through graph_batch17200.c is **not** bar3,
# not Deck Top-50 title runs, and not product score (gj_bar3_ready_17200
# and gj_product_score_17200 remain 0 by design). Soft ≠ product complete.
#
# Decade paste hygiene (17200-era / Wave 30 example)
# ------------------------------------------------
# After CREATE-ONLY wave 17151–17200 exists on disk (optional extension
# 17101–17150 may land ahead of the decade bands):
#
#   # Prefer exist-only so missing TUs never produce dead Makefile lines:
#   ./scripts/gj-continuum-makefile-snippet.sh --exist-only 17151 17200
#   ./scripts/gj-continuum-makefile-snippet.sh --exist-only 17101 17200
#
#   # Or explicit full decade after confirming all 50 TUs present:
#   ./scripts/gj-continuum-makefile-snippet.sh 17151 17200
#
#   # After a prior wire, emit the next N lines past honest makefile_max:
#   ./scripts/gj-continuum-makefile-snippet.sh --next 100
#   ./scripts/gj-continuum-makefile-snippet.sh --next        # default decade 100
#
# Soft continuum bands inside a decade (documentation only; this helper
# does not classify bands — it only emits path lines):
#
#   identity   [M-49, M-25]   gj_batch_id_N + gj_graph_milestone_N
#   fill       [M-24, M-10]   gj_batch_id_N + unique soft unit helper
#   markers    [M-9,  M]      decade soft-gate lamps (host probe surface)
#
# For M=17200 that is identity 17151–17175, fill 17176–17190,
# markers 17191–17200. Continuum extension 17101–17150 sits ahead of
# the decade bands. Prior decade M=17100: identity 17051–17075,
# fill 17076–17090, markers 17091–17100.
# See _gen_milestone_17100.py module docstring (when present) and
# user/libcgj/tests/NOTES_milestone_17100.txt (when present).
#
# Usage:
#   ./scripts/gj-continuum-makefile-snippet.sh 17051 17100
#   ./scripts/gj-continuum-makefile-snippet.sh 17151 17200
#   ./scripts/gj-continuum-makefile-snippet.sh --next 100
#   ./scripts/gj-continuum-makefile-snippet.sh --next          # default decade 100
#   ./scripts/gj-continuum-makefile-snippet.sh --max           # print makefile_max only
#   ./scripts/gj-continuum-makefile-snippet.sh --exist-only 17151 17200
#
# Options:
#   --next [N]       emit N lines after highest graph_batch* in Makefile (default 100)
#   --max            print highest wired batch number and exit (honest scan)
#   --exist-only     only emit lines whose user/libcgj/src/graph_batchN.c exists
#   --tabs           indent with a leading tab (older CGJ_SRCS style)
#   --no-backslash-last  omit trailing \ on the final emitted line
#   -h, --help       usage
#
# Env:
#   GJ_MAKEFILE      Makefile path (default: $ROOT/Makefile)
#   GJ_CGJ_SRC       batch source dir (default: $ROOT/user/libcgj/src)
#
# Stdout vs stderr
# ----------------
#   stdout — paste-ready CGJ_SRCS lines only (or makefile_max=N for --max)
#   stderr — diagnostic banners (makefile_max, emitted/skipped counts)
#            safe to discard when capturing paste text:  … 2>/dev/null
#
# Example paste target (end of CGJ_SRCS before rand48.c):
#   user/libcgj/src/graph_batch17100.c \   ← prior high-water (if still tip)
#   user/libcgj/src/graph_batch17101.c \   ← Wave 30 helper output starts here
#   … through graph_batch17200.c once CREATE-ONLY + parent wire land
#
# Non-claims (keep soft continuum honest)
# ---------------------------------------
#   * Emitting paste lines does not wire the tree until a human/parent
#     pastes them and rebuilds.
#   * makefile_max=N is soft graph parent wire only (scan; not hard-coded).
#   * Soft milestone host gates ≠ Steam client launch ≠ title try.
#   * Matrix Deck Top-50 stays NOT-TRIED until real-DUT title runs.
#   * Product score / bar3-ready lamps remain 0 by design (soft ≠ product).
#
# See also:
#   user/libcgj/src/_gen_milestone_17100.py   (prior Wave 29 CREATE-ONLY emitter)
#   user/libcgj/src/_gen_milestone_17200.py   (Wave 30 CREATE-ONLY emitter, when present)
#   user/libcgj/tests/NOTES_milestone_17100.txt
#   user/libcgj/tests/NOTES_milestone_17200.txt
#   user/libcgj/tests/cgj_soft_milestone_17100.c
#   user/libcgj/tests/cgj_soft_milestone_17200.c
#   docs/GLIBC_COMPAT.md   (graph continuum status)
#   docs/STEAM_HWTEST.md    (continuum soft scope)
#   scripts/gj-product-summary.sh  (side panel: continuum makefile_max=…)
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
MAKEFILE="${GJ_MAKEFILE:-$ROOT/Makefile}"
CGJ_SRC="${GJ_CGJ_SRC:-$ROOT/user/libcgj/src}"

MODE="range" # range | next | max
NEXT_COUNT=100
EXIST_ONLY=0
USE_TABS=0
NO_BSLASH_LAST=0
FROM=""
TO=""

usage() {
	cat <<'EOF' >&2
usage: gj-continuum-makefile-snippet.sh FROM TO
       gj-continuum-makefile-snippet.sh --next [COUNT]
       gj-continuum-makefile-snippet.sh --max
       gj-continuum-makefile-snippet.sh --exist-only FROM TO

Soft continuum parent paste helper (does NOT edit the Makefile).

Emit paste-ready CGJ_SRCS lines:
  user/libcgj/src/graph_batchN.c \

makefile_max honesty:
  --max scans the Makefile for the highest graph_batchN.c already wired.
  Scan has no artificial cap (supports through graph_batch17200.c and beyond).
  High-water is whatever the scan prints (source of truth) — do not hardcode.
  Wave 30 exclusive target decade: M=17200 (soft graph only — not bar3 /
  Steam client / Top-50 titles). Until parent wires 17101–17200, --max may
  still report prior tip (e.g. makefile_max=17100). Soft high-water may
  advance toward 17200 while scan remains at prior tip.

Workflow:
  1. Land CREATE-ONLY TUs (e.g. user/libcgj/src/_gen_milestone_17200.py)
  2. Paste this helper's stdout into CGJ_SRCS (before rand48.c etc.)
  3. Rebuild libcgj; optional cgj_soft_milestone_*.c host probes

Decade bands for milestone M=17200 (docs only; helper emits paths only):
  identity 17151-17175 | fill 17176-17190 | markers 17191-17200
  extension 17101-17150 (pre-decade; may land ahead of identity/fill/markers)
  (prior M=17100: identity 17051-17075 | fill 17076-17090 | markers 17091-17100)

Options:
  --next [N]            after makefile_max, emit N lines (default 100)
  --max                 print highest wired graph_batchN number only
  --exist-only          skip N when source file is missing
  --tabs                leading tab indent
  --no-backslash-last   final line without trailing backslash
  -h, --help            this help

Env:
  GJ_MAKEFILE           Makefile path (default: $ROOT/Makefile)
  GJ_CGJ_SRC            batch source dir (default: $ROOT/user/libcgj/src)

Examples:
  ./scripts/gj-continuum-makefile-snippet.sh --max
  ./scripts/gj-continuum-makefile-snippet.sh --exist-only 17151 17200
  ./scripts/gj-continuum-makefile-snippet.sh --next 100
EOF
}

# Highest graph_batchN.c referenced in Makefile (numeric max).
# Honest scan only — never hard-code a milestone here.
# No artificial upper bound: correctly reports through 16100+ when wired.
makefile_max() {
	if [[ ! -f "$MAKEFILE" ]]; then
		echo "gj-continuum-makefile-snippet: missing Makefile: $MAKEFILE" >&2
		return 1
	fi
	local mx
	mx=$(grep -oE 'graph_batch[0-9]+\.c' "$MAKEFILE" 2>/dev/null \
		| sed -E 's/^graph_batch([0-9]+)\.c$/\1/' \
		| sort -n \
		| tail -n1 || true)
	if [[ -z "${mx:-}" ]]; then
		echo "gj-continuum-makefile-snippet: no graph_batchN.c in $MAKEFILE" >&2
		return 1
	fi
	printf '%s\n' "$mx"
}

while [[ $# -gt 0 ]]; do
	case "$1" in
	-h | --help)
		usage
		exit 0
		;;
	--max)
		MODE="max"
		shift
		;;
	--next)
		MODE="next"
		shift
		if [[ $# -gt 0 && "$1" =~ ^[0-9]+$ ]]; then
			NEXT_COUNT=$1
			shift
		fi
		;;
	--exist-only)
		EXIST_ONLY=1
		shift
		;;
	--tabs)
		USE_TABS=1
		shift
		;;
	--no-backslash-last)
		NO_BSLASH_LAST=1
		shift
		;;
	--)
		shift
		break
		;;
	-*)
		echo "gj-continuum-makefile-snippet: unknown option: $1" >&2
		usage
		exit 2
		;;
	*)
		break
		;;
	esac
done

if [[ "$MODE" == "max" ]]; then
	mx=$(makefile_max)
	echo "makefile_max=$mx"
	exit 0
fi

if [[ "$MODE" == "next" ]]; then
	mx=$(makefile_max)
	FROM=$((mx + 1))
	TO=$((mx + NEXT_COUNT))
	echo "# gj-continuum-makefile-snippet: makefile_max=$mx next=[$FROM,$TO] count=$NEXT_COUNT" >&2
elif [[ $# -eq 2 ]]; then
	FROM=$1
	TO=$2
else
	usage
	exit 2
fi

if ! [[ "$FROM" =~ ^[0-9]+$ && "$TO" =~ ^[0-9]+$ ]]; then
	echo "gj-continuum-makefile-snippet: FROM/TO must be integers (got FROM=$FROM TO=$TO)" >&2
	exit 2
fi
if ((FROM > TO)); then
	echo "gj-continuum-makefile-snippet: FROM ($FROM) > TO ($TO)" >&2
	exit 2
fi

indent=""
if ((USE_TABS)); then
	indent=$'\t'
fi

# Collect N list first so --no-backslash-last and --exist-only work cleanly.
declare -a nums=()
n=$FROM
while ((n <= TO)); do
	src="$CGJ_SRC/graph_batch${n}.c"
	if ((EXIST_ONLY)) && [[ ! -f "$src" ]]; then
		n=$((n + 1))
		continue
	fi
	nums+=("$n")
	n=$((n + 1))
done

emitted=${#nums[@]}
skipped=$((TO - FROM + 1 - emitted))
i=0
while ((i < emitted)); do
	n=${nums[$i]}
	rel="user/libcgj/src/graph_batch${n}.c"
	if ((NO_BSLASH_LAST)) && ((i == emitted - 1)); then
		printf '%s%s\n' "$indent" "$rel"
	else
		printf '%s%s \\\n' "$indent" "$rel"
	fi
	i=$((i + 1))
done

echo "# gj-continuum-makefile-snippet: emitted=$emitted skipped=$skipped range=[$FROM,$TO]" >&2
exit 0
