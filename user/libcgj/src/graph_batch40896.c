/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch40896: smoke soft lamp (wave 40900).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_soft_40900(void);
 *     - Returns 1 (smoke soft surface ready for the milestone 40900
 *       continuum). Soft compile-time product tag; not a live smoke run.
 *   uint32_t __gj_smoke_soft_40900  (alias)
 *   __libcgj_batch40896_marker = "libcgj-batch40896"
 *
 * Milestone 40900 exclusive continuum CREATE-ONLY (40891-40900). Unique
 * gj_smoke_soft_40900 surface only; no multi-def. Distinct from
 * gj_smoke_soft_40800 / gj_smoke_soft_40700 / gj_smoke_soft_40600 and
 * sibling 40900 milestone symbols (dyn_soft, continuum_ready, …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (40891–40900): pairs with static freestanding
 * soft smoke hosts (cgj_smoke / cgj_soft_milestone_40900). Soft rule:
 * missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=40900 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 40896. */
const char __libcgj_batch40896_marker[] = "libcgj-batch40896";

/* Smoke soft ready lamp for wave 40900 (static soft surface present). */
#define B40896_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b40896_soft(void)
{
	return B40896_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_soft_40900 - report smoke soft lamp for wave 40900.
 *
 * Always returns 1u (smoke soft surface ready). Soft pure-data product
 * tag; does not invoke cgj_smoke or any host process. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_smoke_soft_40900(void)
{
	(void)NULL;
	return b40896_soft();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_smoke_soft_40900(void)
    __attribute__((alias("gj_smoke_soft_40900")));
