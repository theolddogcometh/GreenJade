/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch15296: smoke soft lamp (wave 15300).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_soft_15300(void);
 *     - Returns 1 (smoke soft surface ready for the milestone 15300
 *       continuum). Soft compile-time product tag; not a live smoke run.
 *   uint32_t __gj_smoke_soft_15300  (alias)
 *   __libcgj_batch15296_marker = "libcgj-batch15296"
 *
 * Milestone 15300 exclusive continuum CREATE-ONLY (15291-15300). Unique
 * gj_smoke_soft_15300 surface only; no multi-def. Distinct from
 * gj_smoke_soft_15200 / gj_smoke_soft_15100 / gj_smoke_soft_15000 and
 * sibling 15300 milestone symbols (dyn_soft, continuum_ready, …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (15291–15300): pairs with static freestanding
 * soft smoke hosts (cgj_smoke / cgj_soft_milestone_15300). Soft rule:
 * missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=15300 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 15296. */
const char __libcgj_batch15296_marker[] = "libcgj-batch15296";

/* Smoke soft ready lamp for wave 15300 (static soft surface present). */
#define B15296_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b15296_soft(void)
{
	return B15296_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_soft_15300 - report smoke soft lamp for wave 15300.
 *
 * Always returns 1u (smoke soft surface ready). Soft pure-data product
 * tag; does not invoke cgj_smoke or any host process. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_smoke_soft_15300(void)
{
	(void)NULL;
	return b15296_soft();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_smoke_soft_15300(void)
    __attribute__((alias("gj_smoke_soft_15300")));
