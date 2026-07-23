#!/usr/bin/env python3
# SPDX-License-Identifier: MIT OR Apache-2.0
# Copyright (c) 2026 Project GreenJade contributors
"""
Soft continuum generator — MILESTONE 15000 (CREATE-ONLY).

GreenJade pure-C freestanding dual MIT/Apache. This script only *emits*
exclusive continuum graph_batch*.c translation units under
``user/libcgj/src/``. It does **not**:

  - edit the Makefile / CGJ_SRCS (parent wiring is separate)
  - touch graph_batch*.c outside the 14951–15000 wave
  - claim Steam bar3 readiness, Deck Top-50 title runs, or product score
  - hardcode ``makefile_max`` (that value is an honest scan of the Makefile)

Paired helper (parent paste, no auto-edit)
------------------------------------------
  scripts/gj-continuum-makefile-snippet.sh

  Honest high-water mark of wired batches::

    ./scripts/gj-continuum-makefile-snippet.sh --max
    # → makefile_max=15000   when parent has wired through graph_batch15000.c

  Paste-ready CGJ_SRCS lines for this decade (after CREATE-ONLY lands)::

    ./scripts/gj-continuum-makefile-snippet.sh 14951 15000
    ./scripts/gj-continuum-makefile-snippet.sh --exist-only 14951 15000
    ./scripts/gj-continuum-makefile-snippet.sh --next 100   # after makefile_max

Soft continuum model (this wave)
--------------------------------
A continuum *decade* toward milestone M is a closed CREATE-ONLY range of
graph batches ending at M. For M = **15000** the exclusive range is
**14951–15000** (50 TUs), layered as:

  ┌───────────────────┬──────────────┬──────────────────────────────────────┐
  │ Band              │ Batches      │ Surface role                         │
  ├───────────────────┼──────────────┼──────────────────────────────────────┤
  │ Identity          │ 14951–14975  │ gj_batch_id_N + gj_graph_milestone_N │
  │                   │ (25 TUs)     │ (unique per-TU identity lamps)       │
  ├───────────────────┼──────────────┼──────────────────────────────────────┤
  │ Fill soft units   │ 14976–14990  │ gj_batch_id_N + one unique soft unit │
  │                   │ (15 TUs)     │ helper (wave_fill / slot_* / …)      │
  ├───────────────────┼──────────────┼──────────────────────────────────────┤
  │ Milestone markers │ 14991–15000  │ decade soft gates (Wave 8 exclusive) │
  │                   │ (10 TUs)     │ CREATE-ONLY marker/capstone band     │
  └───────────────────┴──────────────┴──────────────────────────────────────┘

Milestone 15000 marker KATs (soft host probe expectations)
----------------------------------------------------------
  Symbol                      Expected   Source TU
  --------------------------  ---------  --------------------
  gj_shell_green_15000        1          graph_batch14991.c
  gj_libcgj_green_15000       1          graph_batch14992.c
  gj_bar3_ready_15000         0          graph_batch14993.c  (bar3 open)
  gj_product_score_15000      0          graph_batch14994.c  (score open)
  gj_continuum_ready_15000    1          graph_batch14995.c
  gj_smoke_soft_15000         1          graph_batch14996.c
  gj_dyn_soft_15000           1          graph_batch14997.c
  gj_milestone_tag_15000      15000      graph_batch14998.c
  gj_continuum_wave_15000     15000      graph_batch14999.c
  gj_batch_id_15000           15000      graph_batch15000.c
  gj_graph_milestone_15000    15000      graph_batch15000.c
  __libcgj_batchNNNNN_marker  string     matching batch TU

Honesty / non-claims
--------------------
  * ``makefile_max=15000`` means parent wired CGJ_SRCS through
    ``graph_batch15000.c``. It is **soft graph growth only**.
  * ``gj_bar3_ready_15000`` is intentionally **0** — Steam client / Deck
    Top-50 title matrix remain open (NOT-TRIED × 50).
  * ``gj_product_score_15000`` is intentionally **0**.
  * Soft rule for host probes: missing symbol → skip; present but wrong
    value → hard fail. Direct-link KATs use ``-DCGJ_SOFT_M15000_DIRECT``.
  * No multi-def: every public symbol is unique to its TU; never redefine
    prior ``gj_*_14900`` / ``gj_*_14800`` / bare ``gj_batch_id`` surfaces.
  * No parent wires inside generated TUs (no Makefile, map, or smoke edits).

CREATE-ONLY contract
--------------------
  Default mode **skips** paths that already exist (safe re-run after the
  wave has landed). Pass ``--force`` only when deliberately rewriting
  these exclusive TUs. This script never deletes files.

  Wave 8 exclusive CREATE-ONLY lands the **markers** band
  (14991–15000) first; identity/fill may be emitted later via
  ``--band identity`` / ``--band fill`` without touching markers if they
  already exist.

Freestanding pure-C surface (every emitted TU)
----------------------------------------------
  * SPDX dual license: MIT OR Apache-2.0
  * Integer-only freestanding C11; ``-ffreestanding -msse2 -Wall -Wextra -Werror``
  * No malloc, errno, libc I/O, third-party paste, or ``__int128``
  * Marker string ``__libcgj_batchN_marker = "libcgj-batchN"``
  * Underscored ``__gj_*`` aliases via ``__attribute__((alias(...)))``

Usage
-----
  # Dry-run (print planned paths + band; write nothing):
  python3 user/libcgj/src/_gen_milestone_15000.py --dry-run

  # CREATE-ONLY emit (skip existing; typical after empty tree for this wave):
  python3 user/libcgj/src/_gen_milestone_15000.py

  # Restrict to marker band only (soft-gate TUs; Wave 8 exclusive):
  python3 user/libcgj/src/_gen_milestone_15000.py --band markers

  # Bands: identity | fill | markers | all  (default: all)

  # Force rewrite (dangerous if hand-edited):
  python3 user/libcgj/src/_gen_milestone_15000.py --force

  # After sources exist, parent pastes CGJ_SRCS (separate step; not this script):
  ./scripts/gj-continuum-makefile-snippet.sh --exist-only 14951 15000

See also
--------
  user/libcgj/src/_gen_milestone_14900.py  (prior decade generator)
  user/libcgj/tests/NOTES_milestone_14900.txt  (prior decade NOTES pattern)
  docs/GLIBC_COMPAT.md  (graph continuum status; makefile_max honesty)
  docs/STEAM_HWTEST.md   (continuum soft scope; bar3 open honesty)
"""

from __future__ import annotations

import argparse
import os
import sys
from pathlib import Path
from typing import Callable, Iterable, List, Sequence, Tuple

# ---------------------------------------------------------------------------
# Wave constants — MILESTONE 15000 soft continuum (honest ceiling)
# ---------------------------------------------------------------------------

MILESTONE = 15000
# Exclusive CREATE-ONLY range for this decade wave (50 TUs).
WAVE_LO = 14951
WAVE_HI = 15000  # inclusive; coincides with MILESTONE

# Band splits (inclusive ranges).
IDENTITY_LO, IDENTITY_HI = 14951, 14975
FILL_LO, FILL_HI = 14976, 14990
MARKERS_LO, MARKERS_HI = 14991, 15000

# Fill soft-unit stem per batch (unique symbol fragment before _u_N).
# Order is fixed to match the decade continuum pattern (wave_fill…anchor).
FILL_UNITS: Sequence[Tuple[int, str]] = (
    (14976, "wave_fill"),
    (14977, "slot_a"),
    (14978, "slot_b"),
    (14979, "slot_c"),
    (14980, "bridge"),
    (14981, "pad_x"),
    (14982, "pad_y"),
    (14983, "pad_z"),
    (14984, "link_soft"),
    (14985, "latch"),
    (14986, "tick"),
    (14987, "tone"),
    (14988, "pulse"),
    (14989, "spark"),
    (14990, "anchor"),
)

# Milestone marker surfaces: (batch, public_stem, return_u32, one-line blurb).
# Stems become gj_<stem>_15000. bar3/product intentionally stay 0.
MARKER_SURFACES: Sequence[Tuple[int, str, int, str]] = (
    (14991, "shell_green", 1, "shell product green extract (wave 15000)"),
    (14992, "libcgj_green", 1, "libcgj product green extract (wave 15000)"),
    (14993, "bar3_ready", 0, "product bar3 readiness gate (wave 15000)"),
    (14994, "product_score", 0, "product score lamp (wave 15000)"),
    (14995, "continuum_ready", 1, "continuum soft-ready lamp (wave 15000)"),
    (14996, "smoke_soft", 1, "static smoke soft lamp (wave 15000)"),
    (14997, "dyn_soft", 1, "dyn/dlopen soft lamp (wave 15000)"),
    (14998, "milestone_tag", MILESTONE, "milestone 15000 product tag"),
    (14999, "continuum_wave", MILESTONE, "continuum wave id tag (wave 15000)"),
    # 15000 is dual-surface (batch_id + graph_milestone); handled specially.
)

HEADER_SPDX = """\
/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
"""


def _here() -> Path:
    return Path(__file__).resolve().parent


def _marker_name(n: int) -> str:
    return f"__libcgj_batch{n}_marker"


def _marker_str(n: int) -> str:
    return f"libcgj-batch{n}"


def render_identity(n: int) -> str:
    """Identity continuum TU: unique batch_id + graph_milestone lamps."""
    assert IDENTITY_LO <= n <= IDENTITY_HI
    mk = _marker_name(n)
    ms = _marker_str(n)
    return f"""{HEADER_SPDX}\
 * Desktop glibc graph batch{n}: continuum CREATE-ONLY batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_{n}(void);
 *     - Returns the compile-time graph batch number for this TU ({n}).
 *   uint32_t gj_graph_milestone_{n}(void);
 *     - Returns the current graph milestone revision ({n}).
 *   uint32_t __gj_batch_id_{n}  (alias)
 *   uint32_t __gj_graph_milestone_{n}  (alias)
 *   {mk} = "{ms}"
 *
 * GreenJade continuum CREATE-ONLY wave (batches {IDENTITY_LO}-{IDENTITY_HI}). Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* or
 * milestone_* symbols - avoid multi-def. No parent wires.
 *
 * Soft continuum only: does not advance bar3 / product score. Parent
 * makefile_max honesty is a Makefile scan (target high-water {MILESTONE}).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char {mk}[] = "{ms}";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b{n}_id(void)
{{
	return {n}u;
}}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_{n} - report this TU's graph batch number.
 *
 * Always returns {n}.
 */
uint32_t
gj_batch_id_{n}(void)
{{
	(void)NULL;
	return b{n}_id();
}}

/*
 * gj_graph_milestone_{n} - report this TU's graph milestone revision.
 *
 * Always returns {n}. Distinct from earlier gj_graph_milestone /
 * gj_graph_milestone_N so all can coexist at link time. No parent wires.
 */
uint32_t
gj_graph_milestone_{n}(void)
{{
	return b{n}_id();
}}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_{n}(void)
    __attribute__((alias("gj_batch_id_{n}")));

uint32_t __gj_graph_milestone_{n}(void)
    __attribute__((alias("gj_graph_milestone_{n}")));
"""


def render_fill(n: int, unit: str) -> str:
    """Fill continuum TU: batch_id + one unique soft unit helper."""
    assert FILL_LO <= n <= FILL_HI
    mk = _marker_name(n)
    ms = _marker_str(n)
    # Human blurb for the unit stem (underscore → space-ish title).
    unit_title = unit.replace("_", " ")
    pub = f"gj_{unit}_u_{n}"
    pub_alias = f"__gj_{unit}_u_{n}"
    helper = f"b{n}_{unit}"
    # Special-case titles for a few stems to match prior decade comments.
    title_map = {
        14976: "continuum wave fill soft unit + batch id",
        14980: "continuum bridge soft unit + batch id",
        14990: "continuum anchor soft unit + batch id",
    }
    title = title_map.get(n, f"continuum {unit_title} soft unit + batch id")
    return f"""{HEADER_SPDX}\
 * Desktop glibc graph batch{n}: {title}.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_{n}(void);
 *     - Returns the compile-time graph batch number for this TU ({n}).
 *   uint32_t {pub}(void);
 *     - Returns 1 (small unique soft helper for continuum fill).
 *   uint32_t __gj_batch_id_{n}  (alias)
 *   uint32_t {pub_alias}  (alias)
 *   {mk} = "{ms}"
 *
 * Exclusive continuum CREATE-ONLY ({FILL_LO}-{WAVE_HI} wave toward MILESTONE
 * {MILESTONE}). Unique surface only; no multi-def. Distinct from prior
 * gj_batch_id_* and sibling soft units. No parent wires.
 * No __int128.
 *
 * Soft continuum only: fill helpers are compile-time lamps, not runtime
 * product probes. bar3 / product score remain open at milestone markers.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char {mk}[] = "{ms}";

/* Batch identity. */
#define B{n}_ID  {n}u
/* Soft unit lamp: always on (compile-time fill tag). */
#define B{n}_UNIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b{n}_id(void)
{{
	return B{n}_ID;
}}

static uint32_t
{helper}(void)
{{
	return B{n}_UNIT;
}}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_{n} - soft continuum surface
 *
 * Always returns {n}u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_{n}(void)
{{
	(void)NULL;
	return b{n}_id();
}}

/*
 * {pub} - small unique soft helper
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
{pub}(void)
{{
	return {helper}();
}}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_{n}(void)
    __attribute__((alias("gj_batch_id_{n}")));

uint32_t {pub_alias}(void)
    __attribute__((alias("{pub}")));
"""


def render_marker_simple(n: int, stem: str, value: int, blurb: str) -> str:
    """Single-lamp milestone marker TU (14991–14999)."""
    assert MARKERS_LO <= n <= 14999
    mk = _marker_name(n)
    ms = _marker_str(n)
    pub = f"gj_{stem}_{MILESTONE}"
    pub_alias = f"__gj_{stem}_{MILESTONE}"
    # Macro / helper naming follows landed TUs (semantic suffix).
    if stem in ("milestone_tag", "continuum_wave"):
        macro = f"B{n}_{stem.upper()}"
        helper = f"b{n}_tag" if stem == "milestone_tag" else f"b{n}_wave"
        lamp_comment = {
            "milestone_tag": f"Soft milestone tag for wave {MILESTONE}.",
            "continuum_wave": f"Soft continuum wave id for wave {MILESTONE}.",
        }[stem]
    elif stem == "bar3_ready":
        macro = f"B{n}_BAR3_READY"
        helper = f"b{n}_ready"
        lamp_comment = f"Bar3 readiness lamp for wave {MILESTONE} (still open)."
    elif stem == "product_score":
        macro = f"B{n}_PRODUCT_SCORE"
        helper = f"b{n}_score"
        lamp_comment = f"Product score lamp for wave {MILESTONE} (still open)."
    elif stem.endswith("_green"):
        macro = f"B{n}_{stem.upper()}"
        helper = f"b{n}_green"
        lamp_comment = f"{stem.replace('_', ' ').title()} lamp for wave {MILESTONE}."
    else:
        macro = f"B{n}_{stem.upper()}"
        helper = f"b{n}_lamp"
        lamp_comment = f"{stem.replace('_', ' ')} lamp for wave {MILESTONE}."

    # Doc blurb for return value.
    if value == 0 and stem == "bar3_ready":
        ret_doc = (
            f"Returns 0 (bar3 readiness not complete for the milestone {MILESTONE}\n"
            f" *       continuum; still open). Soft pure-data readiness lamp for\n"
            f" *       Steam/Deck/Top50."
        )
    elif value == 0 and stem == "product_score":
        ret_doc = (
            f"Returns 0 (product score not complete for the milestone {MILESTONE}\n"
            f" *       continuum; still open). Soft pure-data score lamp."
        )
    elif value == MILESTONE:
        ret_doc = (
            f"Returns the milestone tag for the milestone {MILESTONE} continuum\n"
            f" *       (always {MILESTONE}u). Soft compile-time product identity tag."
            if stem == "milestone_tag"
            else (
                f"Returns the continuum wave id for the milestone {MILESTONE} continuum\n"
                f" *       (always {MILESTONE}u). Soft compile-time wave tag."
            )
        )
    else:
        ret_doc = (
            f"Returns {value} (product {stem.replace('_', ' ')} for the milestone\n"
            f" *       {MILESTONE} continuum). Soft compile-time product status tag; not a\n"
            f" *       runtime probe of live services."
        )

    return f"""{HEADER_SPDX}\
 * Desktop glibc graph batch{n}: {blurb}.
 *
 * Surface (unique symbols):
 *   uint32_t {pub}(void);
 *     - {ret_doc}
 *   uint32_t {pub_alias}  (alias)
 *   {mk} = "{ms}"
 *
 * Milestone {MILESTONE} exclusive continuum CREATE-ONLY ({MARKERS_LO}-{WAVE_HI}). Unique surface
 * only; no multi-def. Distinct from gj_*_14900 / gj_*_14800 / gj_*_14700 milestone
 * surfaces and sibling {MILESTONE} milestone symbols. No parent wires.
 * No __int128.
 *
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max={MILESTONE} when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char {mk}[] = "{ms}";

/* {lamp_comment} */
#define {macro}  {value}u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
{helper}(void)
{{
	return {macro};
}}

/* ---- public surface ---------------------------------------------------- */

/*
 * {pub} - soft continuum surface
 *
 * Always returns {value}u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
{pub}(void)
{{
	(void)NULL;
	return {helper}();
}}

/* ---- underscored alias ------------------------------------------------- */

uint32_t {pub_alias}(void)
    __attribute__((alias("{pub}")));
"""


def render_milestone_capstone() -> str:
    """graph_batch15000.c — batch identity + graph milestone dual surface."""
    n = MILESTONE
    mk = _marker_name(n)
    ms = _marker_str(n)
    return f"""{HEADER_SPDX}\
 * Desktop glibc graph batch{n}: MILESTONE {MILESTONE} batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_{n}(void);
 *     - Returns the compile-time graph batch number for this TU ({n}).
 *   uint32_t gj_graph_milestone_{n}(void);
 *     - Returns the current graph milestone revision ({n}).
 *   uint32_t __gj_batch_id_{n}  (alias)
 *   uint32_t __gj_graph_milestone_{n}  (alias)
 *   {mk} = "{ms}"
 *
 * MILESTONE {MILESTONE} for the exclusive continuum CREATE-ONLY wave
 * (batches 14991-14999: shell_green_{n}, libcgj_green_{n},
 * bar3_ready_{n}, product_score_{n}, continuum_ready_{n},
 * smoke_soft_{n}, dyn_soft_{n}, milestone_tag_{n},
 * continuum_wave_{n}). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols - avoid
 * multi-def. No parent wires.
 *
 * Soft continuum capstone only. Honest parent high-water when wired:
 * makefile_max={MILESTONE}. bar3 remains open (gj_bar3_ready_{n} == 0).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char {mk}[] = "{ms}";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b{n}_id(void)
{{
	return {n}u;
}}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_{n} - report this TU's graph batch number.
 *
 * Always returns {n}.
 */
uint32_t
gj_batch_id_{n}(void)
{{
	(void)NULL;
	return b{n}_id();
}}

/*
 * gj_graph_milestone_{n} - report this TU's graph milestone revision.
 *
 * Always returns {n} (MILESTONE {MILESTONE}). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_{n}(void)
{{
	return b{n}_id();
}}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_{n}(void)
    __attribute__((alias("gj_batch_id_{n}")));

uint32_t __gj_graph_milestone_{n}(void)
    __attribute__((alias("gj_graph_milestone_{n}")));
"""


def _band_jobs(band: str) -> List[Tuple[int, str, Callable[[], str]]]:
    """Return ordered (batch_num, band_label, render_thunk) for the band."""
    jobs: List[Tuple[int, str, Callable[[], str]]] = []

    want_identity = band in ("all", "identity")
    want_fill = band in ("all", "fill")
    want_markers = band in ("all", "markers")

    if want_identity:
        for n in range(IDENTITY_LO, IDENTITY_HI + 1):
            jobs.append((n, "identity", lambda n=n: render_identity(n)))

    if want_fill:
        for n, unit in FILL_UNITS:
            jobs.append((n, "fill", lambda n=n, unit=unit: render_fill(n, unit)))

    if want_markers:
        for n, stem, value, blurb in MARKER_SURFACES:
            jobs.append(
                (
                    n,
                    "markers",
                    lambda n=n, stem=stem, value=value, blurb=blurb: render_marker_simple(
                        n, stem, value, blurb
                    ),
                )
            )
        jobs.append((MILESTONE, "markers", render_milestone_capstone))

    return jobs


def _validate_wave_constants() -> None:
    """Internal honesty checks so docs and ranges cannot drift silently."""
    assert WAVE_HI == MILESTONE
    assert IDENTITY_HI + 1 == FILL_LO
    assert FILL_HI + 1 == MARKERS_LO
    assert MARKERS_HI == WAVE_HI
    assert len(FILL_UNITS) == (FILL_HI - FILL_LO + 1)
    assert [n for n, _ in FILL_UNITS] == list(range(FILL_LO, FILL_HI + 1))
    assert len(MARKER_SURFACES) == 9  # 14991–14999; 15000 is capstone
    assert [n for n, _, _, _ in MARKER_SURFACES] == list(range(MARKERS_LO, MARKERS_HI))
    # bar3 / product score must stay open (0) at this soft milestone.
    bar3 = next(v for n, s, v, _ in MARKER_SURFACES if s == "bar3_ready")
    score = next(v for n, s, v, _ in MARKER_SURFACES if s == "product_score")
    assert bar3 == 0 and score == 0


def emit(
    out_dir: Path,
    band: str,
    dry_run: bool,
    force: bool,
) -> int:
    _validate_wave_constants()
    jobs = _band_jobs(band)
    n_write = 0
    n_skip = 0
    n_plan = 0

    print(
        f"# _gen_milestone_{MILESTONE}: soft continuum CREATE-ONLY "
        f"wave=[{WAVE_LO},{WAVE_HI}] milestone={MILESTONE} band={band}",
        file=sys.stderr,
    )
    print(
        f"# identity=[{IDENTITY_LO},{IDENTITY_HI}] "
        f"fill=[{FILL_LO},{FILL_HI}] "
        f"markers=[{MARKERS_LO},{MARKERS_HI}]",
        file=sys.stderr,
    )
    print(
        f"# honesty: target makefile_max={MILESTONE} (parent scan via "
        f"scripts/gj-continuum-makefile-snippet.sh --max); "
        f"bar3 open; no Makefile edits",
        file=sys.stderr,
    )

    for n, label, thunk in jobs:
        path = out_dir / f"graph_batch{n}.c"
        n_plan += 1
        exists = path.exists()
        if exists and not force:
            n_skip += 1
            print(f"# skip existing [{label}] {path.name}", file=sys.stderr)
            continue
        body = thunk()
        if dry_run:
            action = "would-overwrite" if exists else "would-create"
            print(f"# dry-run {action} [{label}] {path.name} ({len(body)} bytes)")
            n_write += 1
            continue
        path.write_text(body, encoding="utf-8")
        action = "overwrite" if exists else "create"
        print(f"# {action} [{label}] {path.name}", file=sys.stderr)
        n_write += 1

    print(
        f"# _gen_milestone_{MILESTONE}: planned={n_plan} "
        f"written={n_write} skipped_existing={n_skip} "
        f"dry_run={int(dry_run)} force={int(force)}",
        file=sys.stderr,
    )
    if not dry_run and n_write:
        print(
            f"# next (parent, not this script): "
            f"./scripts/gj-continuum-makefile-snippet.sh "
            f"--exist-only {WAVE_LO} {WAVE_HI}",
            file=sys.stderr,
        )
    return 0


def main(argv: Sequence[str] | None = None) -> int:
    p = argparse.ArgumentParser(
        description=(
            f"CREATE-ONLY soft continuum generator for milestone {MILESTONE} "
            f"(batches {WAVE_LO}–{WAVE_HI}). Does not edit Makefile; "
            f"makefile_max honesty is a parent Makefile scan "
            f"(target high-water {MILESTONE})."
        ),
        epilog=(
            "Soft gates only; bar3 still open. Pair with "
            "scripts/gj-continuum-makefile-snippet.sh for CGJ_SRCS paste. "
            "See module docstring for continuum band map and KAT table."
        ),
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    p.add_argument(
        "--band",
        choices=("all", "identity", "fill", "markers"),
        default="all",
        help="which soft continuum band to emit (default: all)",
    )
    p.add_argument(
        "--dry-run",
        action="store_true",
        help="print planned creates/overwrites; write nothing",
    )
    p.add_argument(
        "--force",
        action="store_true",
        help="overwrite existing graph_batchN.c in this wave (default: skip)",
    )
    p.add_argument(
        "--out-dir",
        type=Path,
        default=None,
        help="output directory (default: directory of this script)",
    )
    p.add_argument(
        "--list",
        action="store_true",
        help="list batch numbers and bands, then exit",
    )
    ns = p.parse_args(argv)

    out_dir = ns.out_dir if ns.out_dir is not None else _here()
    if not out_dir.is_dir():
        print(f"_gen_milestone_{MILESTONE}: not a directory: {out_dir}", file=sys.stderr)
        return 2

    if ns.list:
        _validate_wave_constants()
        print(f"milestone={MILESTONE} wave=[{WAVE_LO},{WAVE_HI}]")
        print(f"identity\t{IDENTITY_LO}-{IDENTITY_HI}")
        print(f"fill\t{FILL_LO}-{FILL_HI}")
        for n, unit in FILL_UNITS:
            print(f"  graph_batch{n}.c\tgj_batch_id_{n} + gj_{unit}_u_{n}")
        print(f"markers\t{MARKERS_LO}-{MARKERS_HI}")
        for n, stem, value, _blurb in MARKER_SURFACES:
            print(f"  graph_batch{n}.c\tgj_{stem}_{MILESTONE} -> {value}")
        print(
            f"  graph_batch{MILESTONE}.c\t"
            f"gj_batch_id_{MILESTONE} + gj_graph_milestone_{MILESTONE} -> {MILESTONE}"
        )
        print(
            f"makefile_max_target={MILESTONE} "
            f"(honest scan: scripts/gj-continuum-makefile-snippet.sh --max)"
        )
        print("bar3_ready=0 product_score=0  # still open")
        return 0

    return emit(out_dir=out_dir, band=ns.band, dry_run=ns.dry_run, force=ns.force)


if __name__ == "__main__":
    sys.exit(main())
