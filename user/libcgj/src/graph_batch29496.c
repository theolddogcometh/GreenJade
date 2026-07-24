/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch29496: smoke soft lamp (wave 29500).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_soft_29500(void);
 *     - Returns 1 (smoke soft surface ready for the milestone 29500
 *       continuum). Soft compile-time product tag; not a live smoke run.
 *   uint32_t __gj_smoke_soft_29500  (alias)
 *   __libcgj_batch29496_marker = "libcgj-batch29496"
 *
 * Milestone 29500 exclusive continuum CREATE-ONLY (29491-29500). Unique
 * gj_smoke_soft_29500 surface only; no multi-def. Distinct from
 * gj_smoke_soft_29000 / gj_smoke_soft_28900 / gj_smoke_soft_28800 and
 * sibling 29500 milestone symbols (dyn_soft, continuum_ready, …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (29491–29500): pairs with static freestanding
 * soft smoke hosts (cgj_smoke / cgj_soft_milestone_29500). Soft rule:
 * missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=29500 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 29496. */
const char __libcgj_batch29496_marker[] = "libcgj-batch29496";

/* Smoke soft ready lamp for wave 29500 (static soft surface present). */
#define B29496_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b29496_soft(void)
{
	return B29496_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_soft_29500 - report smoke soft lamp for wave 29500.
 *
 * Always returns 1u (smoke soft surface ready). Soft pure-data product
 * tag; does not invoke cgj_smoke or any host process. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_smoke_soft_29500(void)
{
	(void)NULL;
	return b29496_soft();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_smoke_soft_29500(void)
    __attribute__((alias("gj_smoke_soft_29500")));
