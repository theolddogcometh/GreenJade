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
# *decade* is a closed batch range ending at a milestone M (e.g. M=22600
# → exclusive wave 22501–22600). Soft host probes
# (user/libcgj/tests/cgj_soft_milestone_M.c) KAT the decade marker lamps
# only; identity/fill bands are structural unique surfaces.
#
# This script is the **parent paste half** of that workflow:
#
#   1. CREATE-ONLY sources land first (generators / exclusive agents).
#      Example generator for the 22600 wave (Wave 84 exclusive continuum track):
#        user/libcgj/src/_gen_milestone_22600.py
#        python3 user/libcgj/src/_gen_milestone_22600.py --dry-run
#      (prior Wave 83: _gen_milestone_22500.py / M=22500; Wave 82: _gen_milestone_22400.py / M=22400; Wave 81: _gen_milestone_22300.py / M=22300; Wave 80: _gen_milestone_22200.py / M=22200)
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
# graph_batch22600.c (and beyond) once those basenames are wired.
#
# Wave 84 soft note (22600-era): the exclusive target decade is M=22600
# (CREATE-ONLY 22501–22600; extension identity 22501–22550 ahead of the
# decade identity/fill/marker bands). Soft deepen surfaces this wave
# (docs/userland soft stamps): retscepterangle / retglyphangle (CREATE-ONLY soft
# names only — not product). Until parent pastes those basenames into
# CGJ_SRCS, ``--max`` may still report the prior high-water
# (e.g. makefile_max=22500). **Scan is the source of truth** — do not
# hardcode a false makefile_max here or in side panels. Soft high-water
# may be described as **advancing toward 22600** while the honest scan
# remains at the prior tip.
#
# Verify anytime (stdout is greppable by product-summary side panels):
#
#   ./scripts/gj-continuum-makefile-snippet.sh --max
#   # → makefile_max=N   (honest scan; greppable when N reaches the era)
#
# Do **not** document or assert a higher max than the Makefile scan
# reports. Soft graph growth through graph_batch22600.c is **not** bar3,
# not Deck Top-50 title runs, and not product score (gj_bar3_ready_22600
# and gj_product_score_22600 remain 0 by design). Soft ≠ product complete.
#
# Decade paste hygiene (22600-era / Wave 84 example)
# ------------------------------------------------
# After CREATE-ONLY wave 22501–22600 exists on disk:
#
#   # Prefer exist-only so missing TUs never produce dead Makefile lines:
#   ./scripts/gj-continuum-makefile-snippet.sh --exist-only 22501 22600
#   ./scripts/gj-continuum-makefile-snippet.sh --exist-only 22551 22600
#
#   # Or explicit full decade after confirming all TUs present:
#   ./scripts/gj-continuum-makefile-snippet.sh 22501 22600
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
# For M=22600 that is extension identity 22501–22550, decade identity
# 22551–22575, fill 22576–22590, markers 22591–22600. Prior decade M=22500: identity 22451–22475, fill 22476–22490, markers 22491–22500. Prior decade M=22400: identity 22351–22375, fill 22376–22390, markers 22391–22400. Prior decade M=22300: identity 22251–22275, fill 22276–22290, markers 22291–22300. Prior decade
# M=22200: identity 22151–22175, fill 22176–22190, markers 22191–22200.
# See _gen_milestone_22600.py module docstring (when present) and
# user/libcgj/tests/NOTES_milestone_22300.txt
#   user/libcgj/tests/NOTES_milestone_22500.txt
#   user/libcgj/tests/NOTES_milestone_22600.txt (when present).
#
# Usage:
#   ./scripts/gj-continuum-makefile-snippet.sh 17051 17100
#   ./scripts/gj-continuum-makefile-snippet.sh 22501 22600
#   ./scripts/gj-continuum-makefile-snippet.sh --next 100
#   ./scripts/gj-continuum-makefile-snippet.sh --next          # default decade 100
#   ./scripts/gj-continuum-makefile-snippet.sh --max           # print makefile_max only
#   ./scripts/gj-continuum-makefile-snippet.sh --exist-only 22501 22600
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
#   user/libcgj/src/graph_batch22500.c \   ← prior high-water (if still tip)
#   user/libcgj/src/graph_batch22501.c \   ← Wave 84 helper output starts here
#   … through graph_batch22600.c once CREATE-ONLY + parent wire land
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
#   user/libcgj/src/_gen_milestone_22200.py   (prior Wave 80 CREATE-ONLY emitter, when present)
#   user/libcgj/src/_gen_milestone_22100.py   (prior Wave 79 CREATE-ONLY emitter)
#   user/libcgj/src/_gen_milestone_22300.py   (prior Wave 81 CREATE-ONLY emitter, when present)
#   user/libcgj/src/_gen_milestone_22500.py   (prior Wave 83 CREATE-ONLY emitter, when present)
#   user/libcgj/src/_gen_milestone_22400.py   (prior Wave 82 CREATE-ONLY emitter, when present)
#   user/libcgj/src/_gen_milestone_22600.py   (Wave 84 CREATE-ONLY emitter, when present)
#   user/libcgj/tests/NOTES_milestone_22200.txt
#   user/libcgj/tests/NOTES_milestone_22100.txt
#   user/libcgj/tests/NOTES_milestone_22300.txt
#   user/libcgj/tests/NOTES_milestone_22500.txt
#   user/libcgj/tests/NOTES_milestone_22600.txt
#   user/libcgj/tests/cgj_soft_milestone_22200.c
#   user/libcgj/tests/cgj_soft_milestone_22100.c
#   user/libcgj/tests/cgj_soft_milestone_22300.c
#   user/libcgj/tests/cgj_soft_milestone_22500.c
#   user/libcgj/tests/cgj_soft_milestone_22600.c
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
  Scan has no artificial cap (supports through graph_batch22600.c and beyond).
  High-water is whatever the scan prints (source of truth) — do not hardcode.
  Wave 84 exclusive target decade: M=22600 (soft graph only — not bar3 /
  Steam client / Top-50 titles). Soft deepen surfaces: retscepterangle /
  retglyphangle (CREATE-ONLY soft names only). Until parent wires 22501–22600,
  --max may still report prior tip (e.g. makefile_max=22500). Soft high-water
  may advance toward 22600 while scan remains at prior tip.

Workflow:
  1. Land CREATE-ONLY TUs (e.g. user/libcgj/src/_gen_milestone_22600.py)
  2. Paste this helper's stdout into CGJ_SRCS (before rand48.c etc.)
  3. Rebuild libcgj; optional cgj_soft_milestone_*.c host probes

Decade bands for milestone M=22600 (docs only; helper emits paths only):
  extension 22501-22550 | identity 22551-22575 | fill 22576-22590 | markers 22591-22600
  (prior M=22500: identity 22451-22475 | fill 22476-22490 | markers 22491-22500)
  (prior M=22400: identity 22351-22375 | fill 22376-22390 | markers 22391-22400)
  (prior M=22300: identity 22251-22275 | fill 22276-22290 | markers 22291-22300)
  (prior M=22200: identity 22151-22175 | fill 22176-22190 | markers 22191-22200)

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
  ./scripts/gj-continuum-makefile-snippet.sh --exist-only 22501 22600
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
