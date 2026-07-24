/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch28796: smoke soft lamp (wave 28800).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_soft_28800(void);
 *     - Returns 1 (smoke soft surface ready for the milestone 28800
 *       continuum). Soft compile-time product tag; not a live smoke run.
 *   uint32_t __gj_smoke_soft_28800  (alias)
 *   __libcgj_batch28796_marker = "libcgj-batch28796"
 *
 * Milestone 28800 exclusive continuum CREATE-ONLY (28791-28800). Unique
 * gj_smoke_soft_28800 surface only; no multi-def. Distinct from
 * gj_smoke_soft_28700 / gj_smoke_soft_28600 / gj_smoke_soft_28500 and
 * sibling 28800 milestone symbols (dyn_soft, continuum_ready, …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (28791–28800): pairs with static freestanding
 * soft smoke hosts (cgj_smoke / cgj_soft_milestone_28800). Soft rule:
 * missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=28800 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 28796. */
const char __libcgj_batch28796_marker[] = "libcgj-batch28796";

/* Smoke soft ready lamp for wave 28800 (static soft surface present). */
#define B28796_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b28796_soft(void)
{
	return B28796_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_soft_28800 - report smoke soft lamp for wave 28800.
 *
 * Always returns 1u (smoke soft surface ready). Soft pure-data product
 * tag; does not invoke cgj_smoke or any host process. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_smoke_soft_28800(void)
{
	(void)NULL;
	return b28796_soft();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_smoke_soft_28800(void)
    __attribute__((alias("gj_smoke_soft_28800")));
