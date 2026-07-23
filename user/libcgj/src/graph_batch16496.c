/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch16496: smoke soft lamp (wave 16500).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_soft_16500(void);
 *     - Returns 1 (smoke soft surface ready for the milestone 16500
 *       continuum). Soft compile-time product tag; not a live smoke run.
 *   uint32_t __gj_smoke_soft_16500  (alias)
 *   __libcgj_batch16496_marker = "libcgj-batch16496"
 *
 * Milestone 16500 exclusive continuum CREATE-ONLY (16491-16500). Unique
 * gj_smoke_soft_16500 surface only; no multi-def. Distinct from
 * gj_smoke_soft_16500 / gj_smoke_soft_16300 / gj_smoke_soft_16200 and
 * sibling 16500 milestone symbols (dyn_soft, continuum_ready, …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (16491–16500): pairs with static freestanding
 * soft smoke hosts (cgj_smoke / cgj_soft_milestone_16500). Soft rule:
 * missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=16500 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 16496. */
const char __libcgj_batch16496_marker[] = "libcgj-batch16496";

/* Smoke soft ready lamp for wave 16500 (static soft surface present). */
#define B16496_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b16496_soft(void)
{
	return B16496_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_soft_16500 - report smoke soft lamp for wave 16500.
 *
 * Always returns 1u (smoke soft surface ready). Soft pure-data product
 * tag; does not invoke cgj_smoke or any host process. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_smoke_soft_16500(void)
{
	(void)NULL;
	return b16496_soft();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_smoke_soft_16500(void)
    __attribute__((alias("gj_smoke_soft_16500")));
