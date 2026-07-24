/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch29896: smoke soft lamp (wave 29900).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_soft_29900(void);
 *     - Returns 1 (smoke soft surface ready for the milestone 29900
 *       continuum). Soft compile-time product tag; not a live smoke run.
 *   uint32_t __gj_smoke_soft_29900  (alias)
 *   __libcgj_batch29896_marker = "libcgj-batch29896"
 *
 * Milestone 29900 exclusive continuum CREATE-ONLY (29891-29900). Unique
 * gj_smoke_soft_29900 surface only; no multi-def. Distinct from
 * gj_smoke_soft_29800 / gj_smoke_soft_29700 / gj_smoke_soft_29600 and
 * sibling 29900 milestone symbols (dyn_soft, continuum_ready, …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (29891–29900): pairs with static freestanding
 * soft smoke hosts (cgj_smoke / cgj_soft_milestone_29900). Soft rule:
 * missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=29900 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 29896. */
const char __libcgj_batch29896_marker[] = "libcgj-batch29896";

/* Smoke soft ready lamp for wave 29900 (static soft surface present). */
#define B29896_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b29896_soft(void)
{
	return B29896_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_soft_29900 - report smoke soft lamp for wave 29900.
 *
 * Always returns 1u (smoke soft surface ready). Soft pure-data product
 * tag; does not invoke cgj_smoke or any host process. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_smoke_soft_29900(void)
{
	(void)NULL;
	return b29896_soft();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_smoke_soft_29900(void)
    __attribute__((alias("gj_smoke_soft_29900")));
