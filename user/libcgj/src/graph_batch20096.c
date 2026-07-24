/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch20096: smoke soft lamp (wave 20100).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_soft_20100(void);
 *     - Returns 1 (smoke soft surface ready for the milestone 20100
 *       continuum). Soft compile-time product tag; not a live smoke run.
 *   uint32_t __gj_smoke_soft_20100  (alias)
 *   __libcgj_batch20096_marker = "libcgj-batch20096"
 *
 * Milestone 20100 exclusive continuum CREATE-ONLY (20091-20100). Unique
 * gj_smoke_soft_20100 surface only; no multi-def. Distinct from
 * gj_smoke_soft_20000 / gj_smoke_soft_19900 / gj_smoke_soft_19800 and
 * sibling 20100 milestone symbols (dyn_soft, continuum_ready, …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (20091–20100): pairs with static freestanding
 * soft smoke hosts (cgj_smoke / cgj_soft_milestone_20100). Soft rule:
 * missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=20100 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 20096. */
const char __libcgj_batch20096_marker[] = "libcgj-batch20096";

/* Smoke soft ready lamp for wave 20100 (static soft surface present). */
#define B20096_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b20096_soft(void)
{
	return B20096_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_soft_20100 - report smoke soft lamp for wave 20100.
 *
 * Always returns 1u (smoke soft surface ready). Soft pure-data product
 * tag; does not invoke cgj_smoke or any host process. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_smoke_soft_20100(void)
{
	(void)NULL;
	return b20096_soft();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_smoke_soft_20100(void)
    __attribute__((alias("gj_smoke_soft_20100")));
