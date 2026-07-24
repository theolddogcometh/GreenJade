/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch23896: smoke soft lamp (wave 23900).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_soft_23900(void);
 *     - Returns 1 (smoke soft surface ready for the milestone 23900
 *       continuum). Soft compile-time product tag; not a live smoke run.
 *   uint32_t __gj_smoke_soft_23900  (alias)
 *   __libcgj_batch23896_marker = "libcgj-batch23896"
 *
 * Milestone 23900 exclusive continuum CREATE-ONLY (23891-23900). Unique
 * gj_smoke_soft_23900 surface only; no multi-def. Distinct from
 * gj_smoke_soft_23800 / gj_smoke_soft_23700 / gj_smoke_soft_23600 and
 * sibling 23900 milestone symbols (dyn_soft, continuum_ready, …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (23891–23900): pairs with static freestanding
 * soft smoke hosts (cgj_smoke / cgj_soft_milestone_23900). Soft rule:
 * missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=23900 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 23896. */
const char __libcgj_batch23896_marker[] = "libcgj-batch23896";

/* Smoke soft ready lamp for wave 23900 (static soft surface present). */
#define B23896_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b23896_soft(void)
{
	return B23896_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_soft_23900 - report smoke soft lamp for wave 23900.
 *
 * Always returns 1u (smoke soft surface ready). Soft pure-data product
 * tag; does not invoke cgj_smoke or any host process. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_smoke_soft_23900(void)
{
	(void)NULL;
	return b23896_soft();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_smoke_soft_23900(void)
    __attribute__((alias("gj_smoke_soft_23900")));
