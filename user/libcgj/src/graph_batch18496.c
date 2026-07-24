/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch18496: smoke soft lamp (wave 18500).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_soft_18500(void);
 *     - Returns 1 (smoke soft surface ready for the milestone 18500
 *       continuum). Soft compile-time product tag; not a live smoke run.
 *   uint32_t __gj_smoke_soft_18500  (alias)
 *   __libcgj_batch18496_marker = "libcgj-batch18496"
 *
 * Milestone 18500 exclusive continuum CREATE-ONLY (18491-18500). Unique
 * gj_smoke_soft_18500 surface only; no multi-def. Distinct from
 * gj_smoke_soft_18300 / gj_smoke_soft_18200 / gj_smoke_soft_18100 and
 * sibling 18500 milestone symbols (dyn_soft, continuum_ready, …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (18491–18500): pairs with static freestanding
 * soft smoke hosts (cgj_smoke / cgj_soft_milestone_18500). Soft rule:
 * missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=18500 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 18496. */
const char __libcgj_batch18496_marker[] = "libcgj-batch18496";

/* Smoke soft ready lamp for wave 18500 (static soft surface present). */
#define B18496_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b18496_soft(void)
{
	return B18496_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_soft_18500 - report smoke soft lamp for wave 18500.
 *
 * Always returns 1u (smoke soft surface ready). Soft pure-data product
 * tag; does not invoke cgj_smoke or any host process. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_smoke_soft_18500(void)
{
	(void)NULL;
	return b18496_soft();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_smoke_soft_18500(void)
    __attribute__((alias("gj_smoke_soft_18500")));
