/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch26496: smoke soft lamp (wave 26500).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_soft_26500(void);
 *     - Returns 1 (smoke soft surface ready for the milestone 26500
 *       continuum). Soft compile-time product tag; not a live smoke run.
 *   uint32_t __gj_smoke_soft_26500  (alias)
 *   __libcgj_batch26496_marker = "libcgj-batch26496"
 *
 * Milestone 26500 exclusive continuum CREATE-ONLY (26491-26500). Unique
 * gj_smoke_soft_26500 surface only; no multi-def. Distinct from
 * gj_smoke_soft_26400 / gj_smoke_soft_26300 / gj_smoke_soft_26200 and
 * sibling 26500 milestone symbols (dyn_soft, continuum_ready, …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (26491–26500): pairs with static freestanding
 * soft smoke hosts (cgj_smoke / cgj_soft_milestone_26500). Soft rule:
 * missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=26500 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 26496. */
const char __libcgj_batch26496_marker[] = "libcgj-batch26496";

/* Smoke soft ready lamp for wave 26500 (static soft surface present). */
#define B26496_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b26496_soft(void)
{
	return B26496_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_soft_26500 - report smoke soft lamp for wave 26500.
 *
 * Always returns 1u (smoke soft surface ready). Soft pure-data product
 * tag; does not invoke cgj_smoke or any host process. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_smoke_soft_26500(void)
{
	(void)NULL;
	return b26496_soft();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_smoke_soft_26500(void)
    __attribute__((alias("gj_smoke_soft_26500")));
