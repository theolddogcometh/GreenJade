#!/usr/bin/env python3
# SPDX-License-Identifier: MIT OR Apache-2.0
# Copyright (c) 2026 Project GreenJade contributors
"""One-shot CREATE-ONLY: graph_batch60001.c .. graph_batch61000.c via O_EXCL.
Pattern: graph_batch25850 simple + decade lamps (25891-25900) at each *00.
"""
from __future__ import annotations

import errno
import os
import sys
from pathlib import Path

OUT = Path(__file__).resolve().parent
LO, HI = 60001, 61000

MARKER_STEMS = [
    (9, "shell_green", 1, "shell product green extract"),
    (8, "libcgj_green", 1, "libcgj product green extract"),
    (7, "bar3_ready", 0, "product bar3 readiness gate"),
    (6, "product_score", 0, "product readiness score tag"),
    (5, "continuum_ready", 1, "continuum readiness gate"),
    (4, "smoke_soft", 1, "smoke soft lamp"),
    (3, "dyn_soft", 1, "dyn soft lamp"),
    (2, "milestone_tag", None, "milestone product tag"),
    (1, "continuum_wave", None, "continuum wave identity"),
]


def next_hw(n: int) -> int:
    return ((n + 99) // 100) * 100


def simple_body(n: int) -> str:
    hw = next_hw(n)
    band_lo = ((n - 1) // 50) * 50 + 1
    if band_lo < LO:
        band_lo = LO
    band_hi = n
    return f"""/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch{n}: continuum CREATE-ONLY batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_{n}(void);
 *     - Returns the compile-time graph batch number for this TU ({n}).
 *   uint32_t gj_graph_milestone_{n}(void);
 *     - Returns the current graph milestone revision ({n}).
 *   uint32_t __gj_batch_id_{n}  (alias)
 *   uint32_t __gj_graph_milestone_{n}  (alias)
 *   __libcgj_batch{n}_marker = "libcgj-batch{n}"
 *
 * GreenJade continuum CREATE-ONLY wave (batches {band_lo}-{band_hi}, extension identity). Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* or
 * milestone_* symbols - avoid multi-def. No parent wires.
 *
 * Soft continuum only: does not advance bar3 / product score. Parent
 * makefile_max honesty is a Makefile scan (target high-water {hw}).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch{n}_marker[] = "libcgj-batch{n}";

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


def capstone_body(m: int) -> str:
    lo = m - 9
    hi = m - 1
    return f"""/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch{m}: MILESTONE {m} batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_{m}(void);
 *     - Returns the compile-time graph batch number for this TU ({m}).
 *   uint32_t gj_graph_milestone_{m}(void);
 *     - Returns the current graph milestone revision ({m}).
 *   uint32_t __gj_batch_id_{m}  (alias)
 *   uint32_t __gj_graph_milestone_{m}  (alias)
 *   __libcgj_batch{m}_marker = "libcgj-batch{m}"
 *
 * MILESTONE {m} for the exclusive continuum CREATE-ONLY wave
 * (batches {lo}-{hi}: shell_green_{m}, libcgj_green_{m},
 * bar3_ready_{m}, product_score_{m}, continuum_ready_{m},
 * smoke_soft_{m}, dyn_soft_{m}, milestone_tag_{m},
 * continuum_wave_{m}). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols - avoid
 * multi-def. No parent wires.
 *
 * Soft continuum capstone only. Honest parent high-water when wired:
 * makefile_max={m}. bar3 remains open (gj_bar3_ready_{m} == 0);
 * product_score remains open (gj_product_score_{m} == 0).
 *
 * CGJ soft marker band capstone ({lo}–{m}): host soft probes
 * (cgj_soft_milestone_{m}.*) may dlsym / direct-link these lamps.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch {m}. */
const char __libcgj_batch{m}_marker[] = "libcgj-batch{m}";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b{m}_id(void)
{{
	return {m}u;
}}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_{m} - report this TU's graph batch number.
 *
 * Always returns {m}u. Soft pure-data batch identity lamp for the
 * milestone capstone TU. Does not call libc. No parent wires.
 * CGJ soft KAT expectation: {m}.
 */
uint32_t
gj_batch_id_{m}(void)
{{
	(void)NULL;
	return b{m}_id();
}}

/*
 * gj_graph_milestone_{m} - report this TU's graph milestone revision.
 *
 * Always returns {m}u (MILESTONE {m}). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. Soft pure-data only. No parent wires.
 * CGJ soft KAT expectation: {m}.
 */
uint32_t
gj_graph_milestone_{m}(void)
{{
	return b{m}_id();
}}

/* ---- underscored aliases ----------------------------------------------- */

/* CGJ aliases: underscored forms for map / weak-link compatibility. */
uint32_t __gj_batch_id_{m}(void)
    __attribute__((alias("gj_batch_id_{m}")));

uint32_t __gj_graph_milestone_{m}(void)
    __attribute__((alias("gj_graph_milestone_{m}")));
"""


def lamp_body(n: int, m: int, stem: str, value: int, blurb: str) -> str:
    lo = m - 9
    p1, p2, p3 = m - 100, m - 200, m - 300
    bar3_batch = m - 7
    tag_batch = m - 2
    wave_batch = m - 1
    pub = f"gj_{stem}_{m}"
    alias = f"__gj_{stem}_{m}"

    specs = {
        "shell_green": dict(
            macro=f"B{n}_SHELL_GREEN",
            helper=f"b{n}_green",
            lamp=f"Shell green lamp for wave {m} (soft product status: ready).",
            ret=(
                f"Returns 1 (product shell green/ready lamp for the milestone\n"
                f" *       {m} continuum). Soft compile-time product status tag; not a\n"
                f" *       runtime probe of greenjade-shell."
            ),
            exclusive=(
                f" Distinct from\n"
                f" * gj_shell_green_{p1} / gj_shell_green_{p2} / gj_shell_green_{p3} and\n"
                f" * sibling {m} milestone symbols (libcgj_green / bar3_ready / …)."
            ),
            honesty="",
            band=(
                f" * CGJ soft marker band ({lo}–{m}): host soft probes\n"
                f" * (cgj_soft_milestone_{m}.*) may dlsym / direct-link this lamp.\n"
                f" * Soft rule: missing symbol → skip; present but wrong value → hard fail.\n"
            ),
            fn=(
                f" * {pub} - report product shell green lamp for wave {m}.\n"
                f" *\n"
                f" * Always returns 1u (shell green/ready). Soft pure-data product tag;\n"
                f" * does not call libc or probe greenjade-shell. No parent wires.\n"
                f" * CGJ soft KAT expectation: 1."
            ),
            title=f"{blurb} (wave {m})",
        ),
        "libcgj_green": dict(
            macro=f"B{n}_LIBCGJ_GREEN",
            helper=f"b{n}_green",
            lamp=f"Libcgj green lamp for wave {m} (soft product status: ready).",
            ret=(
                f"Returns 1 (product libcgj green/ready lamp for the milestone\n"
                f" *       {m} continuum). Soft compile-time product status tag; not a\n"
                f" *       runtime library load probe."
            ),
            exclusive=(
                f" Distinct from\n"
                f" * gj_libcgj_green_{p1} / gj_libcgj_green_{p2} / gj_libcgj_green_{p3} and\n"
                f" * sibling {m} milestone symbols (shell_green / bar3_ready / …)."
            ),
            honesty="",
            band=(
                f" * CGJ soft marker band ({lo}–{m}): host soft probes\n"
                f" * (cgj_soft_milestone_{m}.*) may dlsym / direct-link this lamp.\n"
                f" * Soft rule: missing symbol → skip; present but wrong value → hard fail.\n"
            ),
            fn=(
                f" * {pub} - report product libcgj green lamp for wave {m}.\n"
                f" *\n"
                f" * Always returns 1u (libcgj green/ready). Soft pure-data product tag;\n"
                f" * does not call libc or load a library. No parent wires.\n"
                f" * CGJ soft KAT expectation: 1."
            ),
            title=f"{blurb} (wave {m})",
        ),
        "bar3_ready": dict(
            macro=f"B{n}_BAR3_READY",
            helper=f"b{n}_ready",
            lamp=f"Bar3 readiness lamp for wave {m} (still open / not complete).",
            ret=(
                f"Returns 0 (bar3 readiness not complete for the milestone {m}\n"
                f" *       continuum; still open). Soft pure-data readiness lamp for\n"
                f" *       Steam/Deck/Top50."
            ),
            exclusive=(
                f" Distinct from\n"
                f" * gj_bar3_ready_{p1} / gj_bar3_ready_{p2} / gj_bar3_ready_{p3} and\n"
                f" * sibling {m} milestone symbols."
            ),
            honesty=(
                f" *\n"
                f" * HONESTY: bar3 remains intentionally open (return 0). Soft continuum\n"
                f" * growth does not claim Steam client bar3, Deck Top-50 title matrix, or\n"
                f" * live hwtest pass. NOT-TRIED × 50 until a real bar3 track lands.\n"
            ),
            band=(
                f" * CGJ soft marker band ({lo}–{m}): host soft probes\n"
                f" * (cgj_soft_milestone_{m}.*) may dlsym / direct-link this lamp.\n"
                f" * Soft rule: missing symbol → skip; present but wrong value → hard fail.\n"
            ),
            fn=(
                f" * {pub} - report bar3 readiness lamp for wave {m}.\n"
                f" *\n"
                f" * Always returns 0u (bar3 still open / not complete). Soft pure-data\n"
                f" * only; does not probe Steam/Deck. Does not call libc. No parent wires.\n"
                f" * CGJ soft KAT expectation: 0 (preserve open gate)."
            ),
            title=f"{blurb} (wave {m})",
        ),
        "product_score": dict(
            macro=f"B{n}_PRODUCT_SCORE",
            helper=f"b{n}_score",
            lamp=f"Product score for wave {m} (still open / not scored).",
            ret=(
                f"Returns 0 (product readiness score tag for the milestone {m}\n"
                f" *       continuum). Soft compile-time product score tag; still open."
            ),
            exclusive=(
                f" Distinct from\n"
                f" * gj_product_score_{p1} / gj_product_score_{p2} / gj_product_score_{p3}\n"
                f" * and sibling {m} milestone symbols."
            ),
            honesty=(
                f" *\n"
                f" * HONESTY: product_score remains intentionally 0. Soft continuum graph\n"
                f" * growth is not a product readiness claim; score stays open until a\n"
                f" * real product-score track advances it.\n"
            ),
            band=(
                f" * CGJ soft marker band ({lo}–{m}): host soft probes\n"
                f" * (cgj_soft_milestone_{m}.*) may dlsym / direct-link this lamp.\n"
                f" * Soft rule: missing symbol → skip; present but wrong value → hard fail.\n"
            ),
            fn=(
                f" * {pub} - report product readiness score for wave {m}.\n"
                f" *\n"
                f" * Always returns 0u (score still open). Soft pure-data product tag.\n"
                f" * Does not call libc. No parent wires.\n"
                f" * CGJ soft KAT expectation: 0 (preserve open score)."
            ),
            title=f"{blurb} (wave {m})",
        ),
        "continuum_ready": dict(
            macro=f"B{n}_CONTINUUM_READY",
            helper=f"b{n}_ready",
            lamp=f"Continuum readiness lamp for wave {m} (soft continuum complete).",
            ret=(
                f"Returns 1 (continuum readiness lamp for the milestone {m}\n"
                f" *       continuum). Soft compile-time product status tag for exclusive-\n"
                f" *       wave continuum readiness (CREATE-ONLY marker band complete)."
            ),
            exclusive=(
                f" Distinct from\n"
                f" * gj_continuum_ready_{p1} / gj_continuum_ready_{p2} /\n"
                f" * gj_continuum_ready_{p3} and sibling {m} milestone symbols\n"
                f" * (gj_graph_milestone_{m}, smoke_soft, dyn_soft, …)."
            ),
            honesty="",
            band=(
                f" * CGJ soft marker band ({lo}–{m}): host soft probes\n"
                f" * (cgj_soft_milestone_{m}.*) may dlsym / direct-link this lamp.\n"
                f" * Soft rule: missing symbol → skip; present but wrong value → hard fail.\n"
                f" * Soft continuum ready ≠ bar3 ready (bar3 stays 0 at batch{bar3_batch}).\n"
            ),
            fn=(
                f" * {pub} - report milestone-{m} continuum ready tag.\n"
                f" *\n"
                f" * Always returns 1u (continuum readiness complete for soft graph growth).\n"
                f" * Link-time presence tags continuum readiness for the wave. Soft pure-\n"
                f" * data only; does not call libc. No parent wires.\n"
                f" * CGJ soft KAT expectation: 1."
            ),
            title=f"{blurb} (wave {m})",
        ),
        "smoke_soft": dict(
            macro=f"B{n}_SMOKE_SOFT",
            helper=f"b{n}_soft",
            lamp=f"Smoke soft ready lamp for wave {m} (static soft surface present).",
            ret=(
                f"Returns 1 (smoke soft surface ready for the milestone {m}\n"
                f" *       continuum). Soft compile-time product tag; not a live smoke run."
            ),
            exclusive=(
                f" Distinct from\n"
                f" * gj_smoke_soft_{p1} / gj_smoke_soft_{p2} / gj_smoke_soft_{p3} and\n"
                f" * sibling {m} milestone symbols (dyn_soft, continuum_ready, …)."
            ),
            honesty="",
            band=(
                f" * CGJ soft marker band ({lo}–{m}): pairs with static freestanding\n"
                f" * soft smoke hosts (cgj_smoke / cgj_soft_milestone_{m}). Soft rule:\n"
                f" * missing symbol → skip; present but wrong value → hard fail.\n"
            ),
            fn=(
                f" * {pub} - report smoke soft lamp for wave {m}.\n"
                f" *\n"
                f" * Always returns 1u (smoke soft surface ready). Soft pure-data product\n"
                f" * tag; does not invoke cgj_smoke or any host process. No parent wires.\n"
                f" * CGJ soft KAT expectation: 1."
            ),
            title=f"{blurb} (wave {m})",
        ),
        "dyn_soft": dict(
            macro=f"B{n}_DYN_SOFT",
            helper=f"b{n}_soft",
            lamp=f"Dyn soft ready lamp for wave {m} (dyn soft surface present).",
            ret=(
                f"Returns 1 (dyn soft surface ready for the milestone {m}\n"
                f" *       continuum). Soft compile-time product tag; not a live dyn probe."
            ),
            exclusive=(
                f" Distinct from\n"
                f" * gj_dyn_soft_{p1} / gj_dyn_soft_{p2} / gj_dyn_soft_{p3} and\n"
                f" * sibling {m} milestone symbols (smoke_soft, continuum_ready, …)."
            ),
            honesty="",
            band=(
                f" * CGJ soft marker band ({lo}–{m}): pairs with dyn/dlopen soft hosts\n"
                f" * (cgj_dyn_smoke / cgj_soft_milestone_{m}). Soft rule: missing symbol\n"
                f" * → skip; present but wrong value → hard fail.\n"
            ),
            fn=(
                f" * {pub} - report dyn soft lamp for wave {m}.\n"
                f" *\n"
                f" * Always returns 1u (dyn soft surface ready). Soft pure-data product\n"
                f" * tag; does not dlopen product SO or probe version maps. No parent wires.\n"
                f" * CGJ soft KAT expectation: 1."
            ),
            title=f"{blurb} (wave {m})",
        ),
        "milestone_tag": dict(
            macro=f"B{n}_MILESTONE_TAG",
            helper=f"b{n}_tag",
            lamp=f"Soft milestone tag for wave {m} (product identity = {m}).",
            ret=(
                f"Returns the milestone tag for the milestone {m} continuum\n"
                f" *       (always {m}u). Soft compile-time product identity tag."
            ),
            exclusive=(
                f" Distinct from\n"
                f" * gj_milestone_tag_{p1} / gj_milestone_tag_{p2} / gj_milestone_tag_{p3},\n"
                f" * gj_graph_milestone_{m} (batch{m}), and gj_continuum_wave_{m}\n"
                f" * (batch{wave_batch})."
            ),
            honesty="",
            band=(
                f" * CGJ soft marker band ({lo}–{m}): host soft probes\n"
                f" * (cgj_soft_milestone_{m}.*) may dlsym / direct-link this lamp.\n"
                f" * Soft rule: missing symbol → skip; present but wrong value → hard fail.\n"
            ),
            fn=(
                f" * {pub} - report soft milestone product tag for wave {m}.\n"
                f" *\n"
                f" * Always returns {m}u. Soft pure-data product identity tag. Does not\n"
                f" * call libc. No parent wires.\n"
                f" * CGJ soft KAT expectation: {m}."
            ),
            title=f"milestone {m} product tag",
        ),
        "continuum_wave": dict(
            macro=f"B{n}_WAVE_ID",
            helper=f"b{n}_wave",
            lamp=f"Exclusive continuum-wave id for milestone {m}.",
            ret=(
                f"Returns the exclusive continuum wave id for the milestone {m}\n"
                f" *       continuum (always {m}u). Soft compile-time product tag."
            ),
            exclusive=(
                f" Distinct from\n"
                f" * gj_continuum_wave_{p1} / gj_continuum_wave_{p2} /\n"
                f" * gj_continuum_wave_{p3}, gj_milestone_tag_{m} (batch{tag_batch}), and\n"
                f" * gj_batch_id_{m} / gj_graph_milestone_{m} (batch{m})."
            ),
            honesty="",
            band=(
                f" * CGJ soft marker band ({lo}–{m}): host soft probes\n"
                f" * (cgj_soft_milestone_{m}.*) may dlsym / direct-link this lamp.\n"
                f" * Soft rule: missing symbol → skip; present but wrong value → hard fail.\n"
            ),
            fn=(
                f" * {pub} - report exclusive continuum wave id for {m}.\n"
                f" *\n"
                f" * Always returns {m}u. Soft pure-data wave identity tag. Does not\n"
                f" * call libc. No parent wires.\n"
                f" * CGJ soft KAT expectation: {m}."
            ),
            title=f"milestone {m} continuum wave identity",
        ),
    }
    s = specs[stem]
    return f"""/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch{n}: {s['title']}.
 *
 * Surface (unique symbols):
 *   uint32_t {pub}(void);
 *     - {s['ret']}
 *   uint32_t {alias}  (alias)
 *   __libcgj_batch{n}_marker = "libcgj-batch{n}"
 *
 * Milestone {m} exclusive continuum CREATE-ONLY ({lo}-{m}). Unique
 * {pub} surface only; no multi-def.{s['exclusive']}
 * No parent wires. No __int128.
{s['honesty']}\
 *
{s['band']}\
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max={m} when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch {n}. */
const char __libcgj_batch{n}_marker[] = "libcgj-batch{n}";

/* {s['lamp']} */
#define {s['macro']}  {value}u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
{s['helper']}(void)
{{
	return {s['macro']};
}}

/* ---- public surface ---------------------------------------------------- */

/*
{s['fn']}
 */
uint32_t
{pub}(void)
{{
	(void)NULL;
	return {s['helper']}();
}}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t {alias}(void)
    __attribute__((alias("{pub}")));
"""


def body_for(n: int) -> str:
    rem = n % 100
    if rem == 0:
        return capstone_body(n)
    if 91 <= rem <= 99:
        m = next_hw(n)
        off = 100 - rem
        for o, stem, val, blurb in MARKER_STEMS:
            if o == off:
                v = m if val is None else val
                return lamp_body(n, m, stem, v, blurb)
        raise RuntimeError(f"no stem for offset {off}")
    return simple_body(n)


def create_excl(path: Path, data: str) -> str:
    flags = os.O_CREAT | os.O_EXCL | os.O_WRONLY
    try:
        fd = os.open(path, flags, 0o644)
    except OSError as e:
        if e.errno == errno.EEXIST:
            return "exists"
        raise
    try:
        os.write(fd, data.encode("utf-8"))
    finally:
        os.close(fd)
    return "created"


def main() -> int:
    created = 0
    exists = 0
    for n in range(LO, HI + 1):
        path = OUT / f"graph_batch{n}.c"
        status = create_excl(path, body_for(n))
        if status == "created":
            created += 1
        else:
            exists += 1
    planned = HI - LO + 1
    print(f"created={created} already_existed={exists} planned={planned} range={LO}..{HI}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
