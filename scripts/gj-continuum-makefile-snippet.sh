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
# *decade* is a closed batch range ending at a milestone M (e.g. M=25400
# → exclusive wave 25301–25400). Soft host probes
# (user/libcgj/tests/cgj_soft_milestone_M.c) KAT the decade marker lamps
# only; identity/fill bands are structural unique surfaces.
#
# This script is the **parent paste half** of that workflow:
#
#   1. CREATE-ONLY sources land first (generators / exclusive agents).
#      Example generator for the 25400 wave (Wave 112 exclusive continuum track):
#        user/libcgj/src/_gen_milestone_25400.py
#        python3 user/libcgj/src/_gen_milestone_25400.py --dry-run
#      (prior Wave 111: _gen_milestone_25300.py / M=25300; prior Wave 110: _gen_milestone_25200.py / M=25200; prior Wave 109: _gen_milestone_25100.py / M=25100; prior Wave 108: _gen_milestone_25000.py / M=25000; prior Wave 106: _gen_milestone_24800.py / M=24800; prior Wave 105: _gen_milestone_24700.py / M=24700; prior Wave 104: _gen_milestone_24600.py / M=24600; Wave 102: _gen_milestone_24500.py / M=24500; Wave 101: _gen_milestone_24300.py / M=24300; Wave 100: _gen_milestone_24200.py / M=24200; Wave 99: _gen_milestone_24100.py / M=24100; Wave 98: _gen_milestone_24000.py / M=24000; Wave 97: _gen_milestone_23900.py / M=23900; Wave 96: _gen_milestone_23800.py / M=23800; Wave 95: _gen_milestone_23700.py / M=23700; Wave 94: _gen_milestone_23600.py / M=23600; Wave 93: _gen_milestone_23500.py / M=23500; Wave 92: _gen_milestone_23400.py / M=23400; Wave 91: _gen_milestone_23300.py / M=23300; Wave 90: _gen_milestone_23200.py / M=23200; Wave 89: _gen_milestone_23100.py / M=23100; Wave 88: _gen_milestone_23000.py / M=23000; Wave 86: _gen_milestone_22800.py / M=22800; Wave 85: _gen_milestone_22700.py / M=22700; Wave 84: _gen_milestone_22600.py / M=22600; Wave 83: _gen_milestone_22500.py / M=22500; Wave 82: _gen_milestone_22400.py / M=22400; Wave 81: _gen_milestone_22300.py / M=22300)
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
# graph_batch25400.c (and beyond) once those basenames are wired.
#
# Wave 112 soft note (25400-era): the exclusive target decade is M=25400
# (CREATE-ONLY 25301–25400; extension identity 25301–25350 ahead of the
# decade identity/fill/marker bands). Soft deepen surfaces this wave
# (docs/userland soft stamps): retfragmentangle / retvertexangle (CREATE-ONLY soft
# names only — not product). Until parent pastes those basenames into
# CGJ_SRCS, ``--max`` may still report the prior high-water
# (e.g. makefile_max=25300). **Scan is the source of truth** — do not
# hardcode a false makefile_max here or in side panels. Soft high-water
# may be described as **advancing toward 25400** while the honest scan
# remains at the prior tip (often makefile_max=25300).
#
# Verify anytime (stdout is greppable by product-summary side panels):
#
#   ./scripts/gj-continuum-makefile-snippet.sh --max
#   # → makefile_max=N   (honest scan; greppable when N reaches the era)
#
# Do **not** document or assert a higher max than the Makefile scan
# reports. Soft graph growth through graph_batch25400.c is **not** bar3,
# not Deck Top-50 title runs, and not product score (gj_bar3_ready_25400
# and gj_product_score_25400 remain 0 by design). Soft ≠ product complete.
#
# Decade paste hygiene (25400-era / Wave 112 example)
# ------------------------------------------------
# After CREATE-ONLY wave 25301–25400 exists on disk:
#
#   # Prefer exist-only so missing TUs never produce dead Makefile lines:
#   ./scripts/gj-continuum-makefile-snippet.sh --exist-only 25301 25400
#   ./scripts/gj-continuum-makefile-snippet.sh --exist-only 25351 25400
#
#   # Or explicit full decade after confirming all TUs present:
#   ./scripts/gj-continuum-makefile-snippet.sh 25301 25400
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
# For M=25400 that is extension identity 25301–25350, decade identity
# 25351–25375, fill 25376–25390, markers 25391–25400. Prior decade M=25300: identity 25251–25275, fill 25276–25290, markers 25291–25300. Prior decade M=25200: identity 25151–25175, fill 25176–25190, markers 25191–25200. Prior decade M=25100: identity 25051–25075, fill 25076–25090, markers 25091–25100. Prior decade M=25000: identity 24951–24975, fill 24976–24990, markers 24991–25000. Prior decade M=24800: identity 24751–24775, fill 24776–24790, markers 24791–24800. Prior decade M=24700: identity 24651–24675, fill 24676–24690, markers 24691–24700. Prior decade M=24600: identity 24551–24575, fill 24576–24590, markers 24591–24600. Prior decade M=24500: identity 24451–24475, fill 24476–24490, markers 24491–24500. Prior decade M=24400: identity 24351–24375, fill 24376–24390, markers 24391–24400. Prior decade M=24300: identity 24251–24275, fill 24276–24290, markers 24291–24300. Prior decade M=24200: identity 24151–24175, fill 24176–24190, markers 24191–24200. Prior decade M=24100: identity 24051–24075, fill 24076–24090, markers 24091–24100. Prior decade M=23800: identity 23651–23675, fill 23676–23690, markers 23691–23700. Prior decade M=23600: identity 23551–23575, fill 23576–23590, markers 23591–23600. Prior decade M=23500: identity 23451–23475, fill 23476–23490, markers 23491–23500. Prior decade M=23400: identity 23351–23375, fill 23376–23390, markers 23391–23400. Prior decade M=23300: identity 23251–23275, fill 23276–23290, markers 23291–23300. Prior decade M=23200: identity 23151–23175, fill 23176–23190, markers 23191–23200. Prior decade M=23000: identity 22951–22975, fill 22976–22990, markers 22991–23000. Prior decade M=22900: identity 22851–22875, fill 22876–22890, markers 22891–22900. Prior decade M=22800: identity 22751–22775, fill 22776–22790, markers 22791–22800. Prior decade M=22700: identity 22651–22675, fill 22676–22690, markers 22691–22700. Prior decade M=22500: identity 22451–22475, fill 22476–22490, markers 22491–22500. Prior decade M=22400: identity 22351–22375, fill 22376–22390, markers 22391–22400. Prior decade
# M=22300: identity 22251–22275, fill 22276–22290, markers 22291–22300.
# See _gen_milestone_25400.py module docstring (when present) and
# user/libcgj/tests/NOTES_milestone_22300.txt
#   user/libcgj/tests/NOTES_milestone_22700.txt
#   user/libcgj/tests/NOTES_milestone_22800.txt
#   user/libcgj/tests/NOTES_milestone_22900.txt
#   user/libcgj/tests/NOTES_milestone_23000.txt
#   user/libcgj/tests/NOTES_milestone_23300.txt
#   user/libcgj/tests/NOTES_milestone_23400.txt
#   user/libcgj/tests/NOTES_milestone_23500.txt
#   user/libcgj/tests/NOTES_milestone_24000.txt
#   user/libcgj/tests/NOTES_milestone_23800.txt
#   user/libcgj/tests/NOTES_milestone_23700.txt
#   user/libcgj/tests/NOTES_milestone_23600.txt (when present).
#
# Usage:
#   ./scripts/gj-continuum-makefile-snippet.sh 17051 17100
#   ./scripts/gj-continuum-makefile-snippet.sh 24701 24800
#   ./scripts/gj-continuum-makefile-snippet.sh --next 100
#   ./scripts/gj-continuum-makefile-snippet.sh --next          # default decade 100
#   ./scripts/gj-continuum-makefile-snippet.sh --max           # print makefile_max only
#   ./scripts/gj-continuum-makefile-snippet.sh --exist-only 24701 24800
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
#   user/libcgj/src/graph_batch25300.c \   ← prior high-water (if still tip)
#   user/libcgj/src/graph_batch25301.c \   ← Wave 112 helper output starts here
#   … through graph_batch25400.c once CREATE-ONLY + parent wire land
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
#   user/libcgj/src/_gen_milestone_22700.py   (prior Wave 85 CREATE-ONLY emitter, when present)
#   user/libcgj/src/_gen_milestone_22600.py   (prior Wave 84 CREATE-ONLY emitter, when present)
#   user/libcgj/src/_gen_milestone_22500.py   (prior Wave 83 CREATE-ONLY emitter, when present)
#   user/libcgj/src/_gen_milestone_22400.py   (prior Wave 82 CREATE-ONLY emitter, when present)
#   user/libcgj/src/_gen_milestone_25400.py   (Wave 112 CREATE-ONLY emitter, when present)
#   user/libcgj/src/_gen_milestone_25300.py   (prior Wave 111 CREATE-ONLY emitter, when present)
#   user/libcgj/src/_gen_milestone_25200.py   (prior Wave 110 CREATE-ONLY emitter, when present)
#   user/libcgj/src/_gen_milestone_25100.py   (prior Wave 109 CREATE-ONLY emitter, when present)
#   user/libcgj/src/_gen_milestone_24700.py   (prior Wave 105 CREATE-ONLY emitter, when present)
#   user/libcgj/src/_gen_milestone_23700.py   (prior Wave 95 CREATE-ONLY emitter, when present)
#   user/libcgj/src/_gen_milestone_23600.py   (prior Wave 94 CREATE-ONLY emitter, when present)
#   user/libcgj/src/_gen_milestone_23500.py   (prior Wave 93 CREATE-ONLY emitter, when present)
#   user/libcgj/src/_gen_milestone_23400.py   (prior Wave 92 CREATE-ONLY emitter, when present)
#   user/libcgj/src/_gen_milestone_23300.py   (prior Wave 91 CREATE-ONLY emitter, when present)
#   user/libcgj/src/_gen_milestone_23100.py   (prior Wave 89 CREATE-ONLY emitter, when present)
#   user/libcgj/src/_gen_milestone_23000.py   (prior Wave 88 CREATE-ONLY emitter, when present)
#   user/libcgj/src/_gen_milestone_22800.py   (prior Wave 86 CREATE-ONLY emitter, when present)
#   user/libcgj/tests/NOTES_milestone_22200.txt
#   user/libcgj/tests/NOTES_milestone_22100.txt
#   user/libcgj/tests/NOTES_milestone_22300.txt
#   user/libcgj/tests/NOTES_milestone_22700.txt
#   user/libcgj/tests/NOTES_milestone_22500.txt
#   user/libcgj/tests/NOTES_milestone_23000.txt
#   user/libcgj/tests/NOTES_milestone_23300.txt
#   user/libcgj/tests/NOTES_milestone_23400.txt
#   user/libcgj/tests/NOTES_milestone_23500.txt
#   user/libcgj/tests/NOTES_milestone_24000.txt
#   user/libcgj/tests/NOTES_milestone_23700.txt
#   user/libcgj/tests/NOTES_milestone_23600.txt
#   user/libcgj/tests/cgj_soft_milestone_22200.c
#   user/libcgj/tests/cgj_soft_milestone_22100.c
#   user/libcgj/tests/cgj_soft_milestone_22300.c
#   user/libcgj/tests/cgj_soft_milestone_22700.c
#   user/libcgj/tests/cgj_soft_milestone_22500.c
#   user/libcgj/tests/cgj_soft_milestone_22800.c
#   user/libcgj/tests/cgj_soft_milestone_23000.c
#   user/libcgj/tests/cgj_soft_milestone_23300.c
#   user/libcgj/tests/cgj_soft_milestone_23400.c
#   user/libcgj/tests/cgj_soft_milestone_23500.c
#   user/libcgj/tests/cgj_soft_milestone_24000.c
#   user/libcgj/tests/cgj_soft_milestone_23700.c
#   user/libcgj/tests/cgj_soft_milestone_23600.c
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
  Scan has no artificial cap (supports through graph_batch25400.c and beyond).
  High-water is whatever the scan prints (source of truth) — do not hardcode.
  Wave 112 exclusive target decade: M=25400 (soft graph only — not bar3 /
  Steam client / Top-50 titles). Soft deepen surfaces: retfragmentangle /
  retvertexangle (CREATE-ONLY soft names only). Until parent wires 25301–25400,
  --max may still report prior tip (e.g. makefile_max=25300). Soft high-water
  may advance toward 25400 while scan remains at prior tip (often 25300).

Workflow:
  1. Land CREATE-ONLY TUs (e.g. user/libcgj/src/_gen_milestone_25400.py)
  2. Paste this helper's stdout into CGJ_SRCS (before rand48.c etc.)
  3. Rebuild libcgj; optional cgj_soft_milestone_*.c host probes

Decade bands for milestone M=25400 (docs only; helper emits paths only):
  extension 25301-25350 | identity 25351-25375 | fill 25376-25390 | markers 25391-25400
    (prior M=25300: identity 25251-25275 | fill 25276-25290 | markers 25291-25300)
    (prior M=25200: identity 25151-25175 | fill 25176-25190 | markers 25191-25200)
  (prior M=25100: identity 25051-25075 | fill 25076-25090 | markers 25091-25100)
  (prior M=25000: identity 24951-24975 | fill 24976-24990 | markers 24991-25000)
  (prior M=24800: identity 24751-24775 | fill 24776-24790 | markers 24791-24800)
  (prior M=24700: identity 24651-24675 | fill 24676-24690 | markers 24691-24700)
  (prior M=24600: identity 24551-24575 | fill 24576-24590 | markers 24591-24600)
  (prior M=24500: identity 24451-24475 | fill 24476-24490 | markers 24491-24500)
  (prior M=24300: identity 24251-24275 | fill 24276-24290 | markers 24291-24300)
  (prior M=24200: identity 24151-24175 | fill 24176-24190 | markers 24191-24200)
  (prior M=24100: identity 24051-24075 | fill 24076-24090 | markers 24091-24100)
  (prior M=23800: identity 23651-23675 | fill 23676-23690 | markers 23691-23700)
  (prior M=23600: identity 23551-23575 | fill 23576-23590 | markers 23591-23600)
  (prior M=23500: identity 23451-23475 | fill 23476-23490 | markers 23491-23500)
  (prior M=23400: identity 23351-23375 | fill 23376-23390 | markers 23391-23400)
  (prior M=23300: identity 23251-23275 | fill 23276-23290 | markers 23291-23300)
  (prior M=23000: identity 22951-22975 | fill 22976-22990 | markers 22991-23000)
  (prior M=22900: identity 22851-22875 | fill 22876-22890 | markers 22891-22900)
  (prior M=22800: identity 22751-22775 | fill 22776-22790 | markers 22791-22800)
  (prior M=22700: identity 22651-22675 | fill 22676-22690 | markers 22691-22700)
  (prior M=22500: identity 22451-22475 | fill 22476-22490 | markers 22491-22500)
  (prior M=22400: identity 22351-22375 | fill 22376-22390 | markers 22391-22400)
  (prior M=22300: identity 22251-22275 | fill 22276-22290 | markers 22291-22300)

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
  ./scripts/gj-continuum-makefile-snippet.sh --exist-only 24701 24800
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
