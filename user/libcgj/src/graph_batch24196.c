/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch24196: smoke soft lamp (wave 24200).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_soft_24200(void);
 *     - Returns 1 (smoke soft surface ready for the milestone 24200
 *       continuum). Soft compile-time product tag; not a live smoke run.
 *   uint32_t __gj_smoke_soft_24200  (alias)
 *   __libcgj_batch24196_marker = "libcgj-batch24196"
 *
 * Milestone 24200 exclusive continuum CREATE-ONLY (24191-24200). Unique
 * gj_smoke_soft_24200 surface only; no multi-def. Distinct from
 * gj_smoke_soft_24100 / gj_smoke_soft_24000 / gj_smoke_soft_23900 and
 * sibling 24200 milestone symbols (dyn_soft, continuum_ready, …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (24191–24200): pairs with static freestanding
 * soft smoke hosts (cgj_smoke / cgj_soft_milestone_24200). Soft rule:
 * missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=24200 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 24196. */
const char __libcgj_batch24196_marker[] = "libcgj-batch24196";

/* Smoke soft ready lamp for wave 24200 (static soft surface present). */
#define B24196_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b24196_soft(void)
{
	return B24196_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_soft_24200 - report smoke soft lamp for wave 24200.
 *
 * Always returns 1u (smoke soft surface ready). Soft pure-data product
 * tag; does not invoke cgj_smoke or any host process. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_smoke_soft_24200(void)
{
	(void)NULL;
	return b24196_soft();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_smoke_soft_24200(void)
    __attribute__((alias("gj_smoke_soft_24200")));
