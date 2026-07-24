/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch66096: smoke soft lamp (wave 66100).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_soft_66100(void);
 *     - Returns 1 (smoke soft surface ready for the milestone 66100
 *       continuum). Soft compile-time product tag; not a live smoke run.
 *   uint32_t __gj_smoke_soft_66100  (alias)
 *   __libcgj_batch66096_marker = "libcgj-batch66096"
 *
 * Milestone 66100 exclusive continuum CREATE-ONLY (25891-66100). Unique
 * gj_smoke_soft_66100 surface only; no multi-def. Distinct from
 * gj_smoke_soft_25800 / gj_smoke_soft_25700 / gj_smoke_soft_25600 and
 * sibling 66100 milestone symbols (dyn_soft, continuum_ready, …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (25891–66100): pairs with static freestanding
 * soft smoke hosts (cgj_smoke / cgj_soft_milestone_66100). Soft rule:
 * missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=66100 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 66096. */
const char __libcgj_batch66096_marker[] = "libcgj-batch66096";

/* Smoke soft ready lamp for wave 66100 (static soft surface present). */
#define B66096_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b66096_soft(void)
{
	return B66096_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_soft_66100 - report smoke soft lamp for wave 66100.
 *
 * Always returns 1u (smoke soft surface ready). Soft pure-data product
 * tag; does not invoke cgj_smoke or any host process. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_smoke_soft_66100(void)
{
	(void)NULL;
	return b66096_soft();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_smoke_soft_66100(void)
    __attribute__((alias("gj_smoke_soft_66100")));
