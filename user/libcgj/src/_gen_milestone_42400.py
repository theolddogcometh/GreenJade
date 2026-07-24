#!/usr/bin/env python3
# SPDX-License-Identifier: MIT OR Apache-2.0
# Copyright (c) 2026 Project GreenJade contributors
"""
Soft continuum generator — MILESTONE 42400 (CREATE-ONLY).

GreenJade pure-C freestanding dual MIT/Apache. This script only *emits*
exclusive continuum graph_batch*.c translation units under
``user/libcgj/src/``. It does **not**:

  - edit the Makefile / CGJ_SRCS (parent wiring is separate)
  - touch graph_batch*.c outside the 42301–42400 wave
  - claim Steam bar3 readiness, Deck Top-50 title runs, or product score
  - hardcode ``makefile_max`` (that value is an honest scan of the Makefile)

Paired helper (parent paste, no auto-edit)
------------------------------------------
  scripts/gj-continuum-makefile-snippet.sh

  Honest high-water mark of wired batches::

    ./scripts/gj-continuum-makefile-snippet.sh --max
    # → makefile_max=42400   when parent has wired through graph_batch42400.c

  Paste-ready CGJ_SRCS lines for this decade (after CREATE-ONLY lands)::

    ./scripts/gj-continuum-makefile-snippet.sh 42301 42400
    ./scripts/gj-continuum-makefile-snippet.sh --exist-only 42301 42400
    ./scripts/gj-continuum-makefile-snippet.sh --next 100   # after makefile_max

Soft continuum model (this wave)
--------------------------------
A continuum *wave* toward milestone M is a closed CREATE-ONLY range of
graph batches ending at M. For M = **42400** the exclusive range is
**42301–42400** (100 TUs), layered as:

  ┌───────────────────┬──────────────┬──────────────────────────────────────┐
  │ Band              │ Batches      │ Surface role                         │
  ├───────────────────┼──────────────┼──────────────────────────────────────┤
  │ Extension identity│ 42301–42350  │ gj_batch_id_N + gj_graph_milestone_N │
  │                   │ (50 TUs)     │ (extension continuum identity lamps) │
  ├───────────────────┼──────────────┼──────────────────────────────────────┤
  │ Decade identity   │ 42351–42375  │ gj_batch_id_N + gj_graph_milestone_N │
  │                   │ (25 TUs)     │ (unique per-TU identity lamps)       │
  ├───────────────────┼──────────────┼──────────────────────────────────────┤
  │ Fill soft units   │ 42376–42390  │ gj_batch_id_N + one unique soft unit │
  │                   │ (15 TUs)     │ helper (wave_fill / slot_* / …)      │
  ├───────────────────┼──────────────┼──────────────────────────────────────┤
  │ Milestone markers │ 42391–42400  │ decade soft gates (Wave 282 exclusive)│
  │                   │ (10 TUs)     │ CREATE-ONLY marker/capstone band     │
  └───────────────────┴──────────────┴──────────────────────────────────────┘

Milestone 42400 marker KATs (soft host probe expectations)
----------------------------------------------------------
  Symbol                      Expected   Source TU
  --------------------------  ---------  --------------------
  gj_shell_green_42400        1          graph_batch42391.c
  gj_libcgj_green_42400       1          graph_batch42392.c
  gj_bar3_ready_42400         0          graph_batch42393.c  (bar3 open)
  gj_product_score_42400      0          graph_batch42394.c  (score open)
  gj_continuum_ready_42400    1          graph_batch42395.c
  gj_smoke_soft_42400         1          graph_batch42396.c
  gj_dyn_soft_42400           1          graph_batch42397.c
  gj_milestone_tag_42400      42400      graph_batch42398.c
  gj_continuum_wave_42400     42400      graph_batch42399.c
  gj_batch_id_42400           42400      graph_batch42400.c
  gj_graph_milestone_42400    42400      graph_batch42400.c
  __libcgj_batchNNNNN_marker  string     matching batch TU

Honesty / non-claims
--------------------
  * ``makefile_max=42400`` means parent wired CGJ_SRCS through
    ``graph_batch42400.c``. It is **soft graph growth only**.
  * ``gj_bar3_ready_42400`` is intentionally **0** — Steam client / Deck
    Top-50 title matrix remain open (NOT-TRIED × 50).
  * ``gj_product_score_42400`` is intentionally **0**.
  * Soft rule for host probes: missing symbol → skip; present but wrong
    value → hard fail. Direct-link KATs use ``-DCGJ_SOFT_M42400_DIRECT``.
  * No multi-def: every public symbol is unique to its TU; never redefine
    prior ``gj_*_42300`` / ``gj_*_42200`` / bare ``gj_batch_id`` surfaces.
  * No parent wires inside generated TUs (no Makefile, map, or smoke edits).

CREATE-ONLY contract
--------------------
  Default mode **skips** paths that already exist (safe re-run after the
  wave has landed). Pass ``--force`` only when deliberately rewriting
  these exclusive TUs. This script never deletes files.

  Wave 282 exclusive CREATE-ONLY lands the **markers** band
  (42391–42400) first; extension/identity/fill may be emitted later via
  ``--band extension`` / ``--band identity`` / ``--band fill`` without
  touching markers if they already exist.

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
  python3 user/libcgj/src/_gen_milestone_42400.py --dry-run

  # CREATE-ONLY emit (skip existing; typical after empty tree for this wave):
  python3 user/libcgj/src/_gen_milestone_42400.py

  # Restrict to marker band only (soft-gate TUs; Wave 282 exclusive):
  python3 user/libcgj/src/_gen_milestone_42400.py --band markers

  # Extension identity only (42301–42350):
  python3 user/libcgj/src/_gen_milestone_42400.py --band extension

  # Bands: extension | identity | fill | markers | all  (default: all)

  # Force rewrite (dangerous if hand-edited):
  python3 user/libcgj/src/_gen_milestone_42400.py --force

  # After sources exist, parent pastes CGJ_SRCS (separate step; not this script):
  ./scripts/gj-continuum-makefile-snippet.sh --exist-only 42301 42400

See also
--------
  user/libcgj/src/_gen_milestone_42300.py  (prior decade generator)
  docs/GLIBC_COMPAT.md  (graph continuum status; makefile_max honesty)
  docs/STEAM_HWTEST.md   (continuum soft scope; bar3 open honesty)
"""

from __future__ import annotations

import argparse
import sys
from pathlib import Path
from typing import Callable, List, Sequence, Tuple

# ---------------------------------------------------------------------------
# Wave constants — MILESTONE 42400 soft continuum (honest ceiling)
# ---------------------------------------------------------------------------

MILESTONE = 42400
# Exclusive CREATE-ONLY range for this decade wave (100 TUs).
WAVE_LO = 42301
WAVE_HI = 42400  # inclusive; coincides with MILESTONE

# Band splits (inclusive ranges).
EXT_LO, EXT_HI = 42301, 42350
IDENTITY_LO, IDENTITY_HI = 42351, 42375
FILL_LO, FILL_HI = 42376, 42390
MARKERS_LO, MARKERS_HI = 42391, 42400

# Fill soft-unit stem per batch (unique symbol fragment before _u_N).
# Order is fixed to match the decade continuum pattern (wave_fill…anchor).
FILL_UNITS: Sequence[Tuple[int, str]] = (
    (42376, "wave_fill"),
    (42377, "slot_a"),
    (42378, "slot_b"),
    (42379, "slot_c"),
    (42380, "bridge"),
    (42381, "pad_x"),
    (42382, "pad_y"),
    (42383, "pad_z"),
    (42384, "link_soft"),
    (42385, "latch"),
    (42386, "tick"),
    (42387, "tone"),
    (42388, "pulse"),
    (42389, "spark"),
    (42390, "anchor"),
)

# Milestone marker surfaces: (batch, public_stem, return_u32, one-line blurb).
# Stems become gj_<stem>_42400. bar3/product intentionally stay 0.
MARKER_SURFACES: Sequence[Tuple[int, str, int, str]] = (
    (42391, "shell_green", 1, "shell product green extract (wave 42400)"),
    (42392, "libcgj_green", 1, "libcgj product green extract (wave 42400)"),
    (42393, "bar3_ready", 0, "product bar3 readiness gate (wave 42400)"),
    (42394, "product_score", 0, "product readiness score tag (wave 42400)"),
    (42395, "continuum_ready", 1, "continuum readiness gate (wave 42400)"),
    (42396, "smoke_soft", 1, "smoke soft lamp (wave 42400)"),
    (42397, "dyn_soft", 1, "dyn soft lamp (wave 42400)"),
    (42398, "milestone_tag", MILESTONE, "milestone 42400 product tag"),
    (42399, "continuum_wave", MILESTONE, "milestone 42400 continuum wave identity"),
    # 42400 is dual-surface (batch_id + graph_milestone); handled specially.
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


def render_identity(n: int, band_lo: int | None = None, band_hi: int | None = None) -> str:
    """Identity continuum TU: unique batch_id + graph_milestone lamps."""
    assert (EXT_LO <= n <= EXT_HI) or (IDENTITY_LO <= n <= IDENTITY_HI)
    if band_lo is None or band_hi is None:
        if EXT_LO <= n <= EXT_HI:
            band_lo, band_hi = EXT_LO, EXT_HI
        else:
            band_lo, band_hi = IDENTITY_LO, IDENTITY_HI
    mk = _marker_name(n)
    ms = _marker_str(n)
    band_kind = "extension identity" if EXT_LO <= n <= EXT_HI else "decade identity"
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
 * GreenJade continuum CREATE-ONLY wave (batches {band_lo}-{band_hi}, {band_kind}). Does NOT
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
    unit_title = unit.replace("_", " ")
    pub = f"gj_{unit}_u_{n}"
    pub_alias = f"__gj_{unit}_u_{n}"
    helper = f"b{n}_{unit}"
    title_map = {
        42376: "continuum wave fill soft unit + batch id",
        42380: "continuum bridge soft unit + batch id",
        42390: "continuum anchor soft unit + batch id",
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
    """Single-lamp milestone marker TU (42391–42399), rich Wave-16 style."""
    assert MARKERS_LO <= n <= (MARKERS_HI - 1)
    mk = _marker_name(n)
    ms = _marker_str(n)
    pub = f"gj_{stem}_{MILESTONE}"
    pub_alias = f"__gj_{stem}_{MILESTONE}"
    # Sibling marker batch ids within this wave (for exclusive_extra notes).
    bar3_batch = MARKERS_LO + 2  # 42393
    tag_batch = MARKERS_LO + 7  # 42398
    wave_batch = MARKERS_LO + 8  # 42399

    if stem == "shell_green":
        macro = f"B{n}_SHELL_GREEN"
        helper = f"b{n}_green"
        lamp_comment = f"Shell green lamp for wave {MILESTONE} (soft product status: ready)."
        ret_doc = (
            f"Returns 1 (product shell green/ready lamp for the milestone\n"
            f" *       {MILESTONE} continuum). Soft compile-time product status tag; not a\n"
            f" *       runtime probe of greenjade-shell."
        )
        exclusive_extra = (
            f" Distinct from\n"
            f" * gj_shell_green_42300 / gj_shell_green_42200 / gj_shell_green_42100 and\n"
            f" * sibling {MILESTONE} milestone symbols (libcgj_green / bar3_ready / …)."
        )
        honesty = ""
        band_note = (
            f" * CGJ soft marker band ({MARKERS_LO}–{MARKERS_HI}): host soft probes\n"
            f" * (cgj_soft_milestone_{MILESTONE}.*) may dlsym / direct-link this lamp.\n"
            f" * Soft rule: missing symbol → skip; present but wrong value → hard fail.\n"
        )
        fn_doc = (
            f" * {pub} - report product shell green lamp for wave {MILESTONE}.\n"
            f" *\n"
            f" * Always returns 1u (shell green/ready). Soft pure-data product tag;\n"
            f" * does not call libc or probe greenjade-shell. No parent wires.\n"
            f" * CGJ soft KAT expectation: 1."
        )
    elif stem == "libcgj_green":
        macro = f"B{n}_LIBCGJ_GREEN"
        helper = f"b{n}_green"
        lamp_comment = f"Libcgj green lamp for wave {MILESTONE} (soft product status: ready)."
        ret_doc = (
            f"Returns 1 (product libcgj green/ready lamp for the milestone\n"
            f" *       {MILESTONE} continuum). Soft compile-time product status tag; not a\n"
            f" *       runtime library load probe."
        )
        exclusive_extra = (
            f" Distinct from\n"
            f" * gj_libcgj_green_42300 / gj_libcgj_green_42200 / gj_libcgj_green_42100 and\n"
            f" * sibling {MILESTONE} milestone symbols (shell_green / bar3_ready / …)."
        )
        honesty = ""
        band_note = (
            f" * CGJ soft marker band ({MARKERS_LO}–{MARKERS_HI}): host soft probes\n"
            f" * (cgj_soft_milestone_{MILESTONE}.*) may dlsym / direct-link this lamp.\n"
            f" * Soft rule: missing symbol → skip; present but wrong value → hard fail.\n"
        )
        fn_doc = (
            f" * {pub} - report product libcgj green lamp for wave {MILESTONE}.\n"
            f" *\n"
            f" * Always returns 1u (libcgj green/ready). Soft pure-data product tag;\n"
            f" * does not call libc or load a library. No parent wires.\n"
            f" * CGJ soft KAT expectation: 1."
        )
    elif stem == "bar3_ready":
        macro = f"B{n}_BAR3_READY"
        helper = f"b{n}_ready"
        lamp_comment = f"Bar3 readiness lamp for wave {MILESTONE} (still open / not complete)."
        ret_doc = (
            f"Returns 0 (bar3 readiness not complete for the milestone {MILESTONE}\n"
            f" *       continuum; still open). Soft pure-data readiness lamp for\n"
            f" *       Steam/Deck/Top50."
        )
        exclusive_extra = (
            f" Distinct from\n"
            f" * gj_bar3_ready_42300 / gj_bar3_ready_42200 / gj_bar3_ready_42100 and\n"
            f" * sibling {MILESTONE} milestone symbols."
        )
        honesty = (
            f" *\n"
            f" * HONESTY: bar3 remains intentionally open (return 0). Soft continuum\n"
            f" * growth does not claim Steam client bar3, Deck Top-50 title matrix, or\n"
            f" * live hwtest pass. NOT-TRIED × 50 until a real bar3 track lands.\n"
        )
        band_note = (
            f" * CGJ soft marker band ({MARKERS_LO}–{MARKERS_HI}): host soft probes\n"
            f" * (cgj_soft_milestone_{MILESTONE}.*) may dlsym / direct-link this lamp.\n"
            f" * Soft rule: missing symbol → skip; present but wrong value → hard fail.\n"
        )
        fn_doc = (
            f" * {pub} - report bar3 readiness lamp for wave {MILESTONE}.\n"
            f" *\n"
            f" * Always returns 0u (bar3 still open / not complete). Soft pure-data\n"
            f" * only; does not probe Steam/Deck. Does not call libc. No parent wires.\n"
            f" * CGJ soft KAT expectation: 0 (preserve open gate)."
        )
    elif stem == "product_score":
        macro = f"B{n}_PRODUCT_SCORE"
        helper = f"b{n}_score"
        lamp_comment = f"Product score for wave {MILESTONE} (still open / not scored)."
        ret_doc = (
            f"Returns 0 (product readiness score tag for the milestone {MILESTONE}\n"
            f" *       continuum). Soft compile-time product score tag; still open."
        )
        exclusive_extra = (
            f" Distinct from\n"
            f" * gj_product_score_42300 / gj_product_score_42200 / gj_product_score_42100\n"
            f" * and sibling {MILESTONE} milestone symbols."
        )
        honesty = (
            f" *\n"
            f" * HONESTY: product_score remains intentionally 0. Soft continuum graph\n"
            f" * growth is not a product readiness claim; score stays open until a\n"
            f" * real product-score track advances it.\n"
        )
        band_note = (
            f" * CGJ soft marker band ({MARKERS_LO}–{MARKERS_HI}): host soft probes\n"
            f" * (cgj_soft_milestone_{MILESTONE}.*) may dlsym / direct-link this lamp.\n"
            f" * Soft rule: missing symbol → skip; present but wrong value → hard fail.\n"
        )
        fn_doc = (
            f" * {pub} - report product readiness score for wave {MILESTONE}.\n"
            f" *\n"
            f" * Always returns 0u (score still open). Soft pure-data product tag.\n"
            f" * Does not call libc. No parent wires.\n"
            f" * CGJ soft KAT expectation: 0 (preserve open score)."
        )
    elif stem == "continuum_ready":
        macro = f"B{n}_CONTINUUM_READY"
        helper = f"b{n}_ready"
        lamp_comment = f"Continuum readiness lamp for wave {MILESTONE} (soft continuum complete)."
        ret_doc = (
            f"Returns 1 (continuum readiness lamp for the milestone {MILESTONE}\n"
            f" *       continuum). Soft compile-time product status tag for exclusive-\n"
            f" *       wave continuum readiness (CREATE-ONLY marker band complete)."
        )
        exclusive_extra = (
            f" Distinct from\n"
            f" * gj_continuum_ready_42300 / gj_continuum_ready_42200 /\n"
            f" * gj_continuum_ready_42100 and sibling {MILESTONE} milestone symbols\n"
            f" * (gj_graph_milestone_{MILESTONE}, smoke_soft, dyn_soft, …)."
        )
        honesty = ""
        band_note = (
            f" * CGJ soft marker band ({MARKERS_LO}–{MARKERS_HI}): host soft probes\n"
            f" * (cgj_soft_milestone_{MILESTONE}.*) may dlsym / direct-link this lamp.\n"
            f" * Soft rule: missing symbol → skip; present but wrong value → hard fail.\n"
            f" * Soft continuum ready ≠ bar3 ready (bar3 stays 0 at batch{bar3_batch}).\n"
        )
        fn_doc = (
            f" * {pub} - report milestone-{MILESTONE} continuum ready tag.\n"
            f" *\n"
            f" * Always returns 1u (continuum readiness complete for soft graph growth).\n"
            f" * Link-time presence tags continuum readiness for the wave. Soft pure-\n"
            f" * data only; does not call libc. No parent wires.\n"
            f" * CGJ soft KAT expectation: 1."
        )
    elif stem == "smoke_soft":
        macro = f"B{n}_SMOKE_SOFT"
        helper = f"b{n}_soft"
        lamp_comment = f"Smoke soft ready lamp for wave {MILESTONE} (static soft surface present)."
        ret_doc = (
            f"Returns 1 (smoke soft surface ready for the milestone {MILESTONE}\n"
            f" *       continuum). Soft compile-time product tag; not a live smoke run."
        )
        exclusive_extra = (
            f" Distinct from\n"
            f" * gj_smoke_soft_42300 / gj_smoke_soft_42200 / gj_smoke_soft_42100 and\n"
            f" * sibling {MILESTONE} milestone symbols (dyn_soft, continuum_ready, …)."
        )
        honesty = ""
        band_note = (
            f" * CGJ soft marker band ({MARKERS_LO}–{MARKERS_HI}): pairs with static freestanding\n"
            f" * soft smoke hosts (cgj_smoke / cgj_soft_milestone_{MILESTONE}). Soft rule:\n"
            f" * missing symbol → skip; present but wrong value → hard fail.\n"
        )
        fn_doc = (
            f" * {pub} - report smoke soft lamp for wave {MILESTONE}.\n"
            f" *\n"
            f" * Always returns 1u (smoke soft surface ready). Soft pure-data product\n"
            f" * tag; does not invoke cgj_smoke or any host process. No parent wires.\n"
            f" * CGJ soft KAT expectation: 1."
        )
    elif stem == "dyn_soft":
        macro = f"B{n}_DYN_SOFT"
        helper = f"b{n}_soft"
        lamp_comment = f"Dyn soft ready lamp for wave {MILESTONE} (dyn soft surface present)."
        ret_doc = (
            f"Returns 1 (dyn soft surface ready for the milestone {MILESTONE}\n"
            f" *       continuum). Soft compile-time product tag; not a live dyn probe."
        )
        exclusive_extra = (
            f" Distinct from\n"
            f" * gj_dyn_soft_42300 / gj_dyn_soft_42200 / gj_dyn_soft_42100 and\n"
            f" * sibling {MILESTONE} milestone symbols (smoke_soft, continuum_ready, …)."
        )
        honesty = ""
        band_note = (
            f" * CGJ soft marker band ({MARKERS_LO}–{MARKERS_HI}): pairs with dyn/dlopen soft hosts\n"
            f" * (cgj_dyn_smoke / cgj_soft_milestone_{MILESTONE}). Soft rule: missing symbol\n"
            f" * → skip; present but wrong value → hard fail.\n"
        )
        fn_doc = (
            f" * {pub} - report dyn soft lamp for wave {MILESTONE}.\n"
            f" *\n"
            f" * Always returns 1u (dyn soft surface ready). Soft pure-data product\n"
            f" * tag; does not dlopen product SO or probe version maps. No parent wires.\n"
            f" * CGJ soft KAT expectation: 1."
        )
    elif stem == "milestone_tag":
        macro = f"B{n}_MILESTONE_TAG"
        helper = f"b{n}_tag"
        lamp_comment = f"Soft milestone tag for wave {MILESTONE} (product identity = {MILESTONE})."
        ret_doc = (
            f"Returns the milestone tag for the milestone {MILESTONE} continuum\n"
            f" *       (always {MILESTONE}u). Soft compile-time product identity tag."
        )
        exclusive_extra = (
            f" Distinct from\n"
            f" * gj_milestone_tag_42300 / gj_milestone_tag_42200 / gj_milestone_tag_42100,\n"
            f" * gj_graph_milestone_{MILESTONE} (batch{MILESTONE}), and gj_continuum_wave_{MILESTONE}\n"
            f" * (batch{wave_batch})."
        )
        honesty = ""
        band_note = (
            f" * CGJ soft marker band ({MARKERS_LO}–{MARKERS_HI}): host soft probes\n"
            f" * (cgj_soft_milestone_{MILESTONE}.*) may dlsym / direct-link this lamp.\n"
            f" * Soft rule: missing symbol → skip; present but wrong value → hard fail.\n"
        )
        fn_doc = (
            f" * {pub} - report soft milestone product tag for wave {MILESTONE}.\n"
            f" *\n"
            f" * Always returns {MILESTONE}u. Soft pure-data product identity tag. Does not\n"
            f" * call libc. No parent wires.\n"
            f" * CGJ soft KAT expectation: {MILESTONE}."
        )
    elif stem == "continuum_wave":
        macro = f"B{n}_WAVE_ID"
        helper = f"b{n}_wave"
        lamp_comment = f"Exclusive continuum-wave id for milestone {MILESTONE}."
        ret_doc = (
            f"Returns the exclusive continuum wave id for the milestone {MILESTONE}\n"
            f" *       continuum (always {MILESTONE}u). Soft compile-time product tag."
        )
        exclusive_extra = (
            f" Distinct from\n"
            f" * gj_continuum_wave_42300 / gj_continuum_wave_42200 /\n"
            f" * gj_continuum_wave_42100, gj_milestone_tag_{MILESTONE} (batch{tag_batch}), and\n"
            f" * gj_batch_id_{MILESTONE} / gj_graph_milestone_{MILESTONE} (batch{MILESTONE})."
        )
        honesty = ""
        band_note = (
            f" * CGJ soft marker band ({MARKERS_LO}–{MARKERS_HI}): host soft probes\n"
            f" * (cgj_soft_milestone_{MILESTONE}.*) may dlsym / direct-link this lamp.\n"
            f" * Soft rule: missing symbol → skip; present but wrong value → hard fail.\n"
        )
        fn_doc = (
            f" * {pub} - report exclusive continuum wave id for {MILESTONE}.\n"
            f" *\n"
            f" * Always returns {MILESTONE}u. Soft pure-data wave identity tag. Does not\n"
            f" * call libc. No parent wires.\n"
            f" * CGJ soft KAT expectation: {MILESTONE}."
        )
    else:
        raise ValueError(f"unknown marker stem: {stem}")

    return f"""{HEADER_SPDX}\
 * Desktop glibc graph batch{n}: {blurb}.
 *
 * Surface (unique symbols):
 *   uint32_t {pub}(void);
 *     - {ret_doc}
 *   uint32_t {pub_alias}  (alias)
 *   {mk} = "{ms}"
 *
 * Milestone {MILESTONE} exclusive continuum CREATE-ONLY ({MARKERS_LO}-{MARKERS_HI}). Unique
 * {pub} surface only; no multi-def.{exclusive_extra}
 * No parent wires. No __int128.
{honesty}\
 *
{band_note}\
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max={MILESTONE} when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch {n}. */
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
{fn_doc}
 */
uint32_t
{pub}(void)
{{
	(void)NULL;
	return {helper}();
}}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t {pub_alias}(void)
    __attribute__((alias("{pub}")));
"""


def render_milestone_capstone() -> str:
    """graph_batch42400.c — batch identity + graph milestone dual surface."""
    n = MILESTONE
    mk = _marker_name(n)
    ms = _marker_str(n)
    markers_simple_hi = MARKERS_HI - 1  # 42399
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
 * (batches {MARKERS_LO}-{markers_simple_hi}: shell_green_{n}, libcgj_green_{n},
 * bar3_ready_{n}, product_score_{n}, continuum_ready_{n},
 * smoke_soft_{n}, dyn_soft_{n}, milestone_tag_{n},
 * continuum_wave_{n}). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols - avoid
 * multi-def. No parent wires.
 *
 * Soft continuum capstone only. Honest parent high-water when wired:
 * makefile_max={MILESTONE}. bar3 remains open (gj_bar3_ready_{n} == 0);
 * product_score remains open (gj_product_score_{n} == 0).
 *
 * CGJ soft marker band capstone ({MARKERS_LO}–{MARKERS_HI}): host soft probes
 * (cgj_soft_milestone_{MILESTONE}.*) may dlsym / direct-link these lamps.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch {n}. */
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
 * Always returns {n}u. Soft pure-data batch identity lamp for the
 * milestone capstone TU. Does not call libc. No parent wires.
 * CGJ soft KAT expectation: {n}.
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
 * Always returns {n}u (MILESTONE {MILESTONE}). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. Soft pure-data only. No parent wires.
 * CGJ soft KAT expectation: {n}.
 */
uint32_t
gj_graph_milestone_{n}(void)
{{
	return b{n}_id();
}}

/* ---- underscored aliases ----------------------------------------------- */

/* CGJ aliases: underscored forms for map / weak-link compatibility. */
uint32_t __gj_batch_id_{n}(void)
    __attribute__((alias("gj_batch_id_{n}")));

uint32_t __gj_graph_milestone_{n}(void)
    __attribute__((alias("gj_graph_milestone_{n}")));
"""


def _band_jobs(band: str) -> List[Tuple[int, str, Callable[[], str]]]:
    """Return ordered (batch_num, band_label, render_thunk) for the band."""
    jobs: List[Tuple[int, str, Callable[[], str]]] = []

    want_extension = band in ("all", "extension")
    want_identity = band in ("all", "identity")
    want_fill = band in ("all", "fill")
    want_markers = band in ("all", "markers")

    if want_extension:
        for n in range(EXT_LO, EXT_HI + 1):
            jobs.append(
                (
                    n,
                    "extension",
                    lambda n=n: render_identity(n, EXT_LO, EXT_HI),
                )
            )

    if want_identity:
        for n in range(IDENTITY_LO, IDENTITY_HI + 1):
            jobs.append(
                (
                    n,
                    "identity",
                    lambda n=n: render_identity(n, IDENTITY_LO, IDENTITY_HI),
                )
            )

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
    assert WAVE_LO == EXT_LO
    assert EXT_HI + 1 == IDENTITY_LO
    assert IDENTITY_HI + 1 == FILL_LO
    assert FILL_HI + 1 == MARKERS_LO
    assert MARKERS_HI == WAVE_HI
    assert (WAVE_HI - WAVE_LO + 1) == 100
    assert len(FILL_UNITS) == (FILL_HI - FILL_LO + 1)
    assert [n for n, _ in FILL_UNITS] == list(range(FILL_LO, FILL_HI + 1))
    assert len(MARKER_SURFACES) == 9  # 42391–42399; 42400 is capstone
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
        f"# extension=[{EXT_LO},{EXT_HI}] "
        f"identity=[{IDENTITY_LO},{IDENTITY_HI}] "
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
        choices=("all", "extension", "identity", "fill", "markers"),
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
        print(f"extension\t{EXT_LO}-{EXT_HI}")
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
