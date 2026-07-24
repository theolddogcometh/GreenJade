/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch31496: smoke soft lamp (wave 31500).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_soft_31500(void);
 *     - Returns 1 (smoke soft surface ready for the milestone 31500
 *       continuum). Soft compile-time product tag; not a live smoke run.
 *   uint32_t __gj_smoke_soft_31500  (alias)
 *   __libcgj_batch31496_marker = "libcgj-batch31496"
 *
 * Milestone 31500 exclusive continuum CREATE-ONLY (31491-31500). Unique
 * gj_smoke_soft_31500 surface only; no multi-def. Distinct from
 * gj_smoke_soft_31400 / gj_smoke_soft_31300 / gj_smoke_soft_31200 and
 * sibling 31500 milestone symbols (dyn_soft, continuum_ready, …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (31491–31500): pairs with static freestanding
 * soft smoke hosts (cgj_smoke / cgj_soft_milestone_31500). Soft rule:
 * missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=31500 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 31496. */
const char __libcgj_batch31496_marker[] = "libcgj-batch31496";

/* Smoke soft ready lamp for wave 31500 (static soft surface present). */
#define B31496_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b31496_soft(void)
{
	return B31496_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_soft_31500 - report smoke soft lamp for wave 31500.
 *
 * Always returns 1u (smoke soft surface ready). Soft pure-data product
 * tag; does not invoke cgj_smoke or any host process. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_smoke_soft_31500(void)
{
	(void)NULL;
	return b31496_soft();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_smoke_soft_31500(void)
    __attribute__((alias("gj_smoke_soft_31500")));
