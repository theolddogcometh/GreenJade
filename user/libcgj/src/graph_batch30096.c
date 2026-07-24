/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch30096: smoke soft lamp (wave 30100).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_soft_30100(void);
 *     - Returns 1 (smoke soft surface ready for the milestone 30100
 *       continuum). Soft compile-time product tag; not a live smoke run.
 *   uint32_t __gj_smoke_soft_30100  (alias)
 *   __libcgj_batch30096_marker = "libcgj-batch30096"
 *
 * Milestone 30100 exclusive continuum CREATE-ONLY (30091-30100). Unique
 * gj_smoke_soft_30100 surface only; no multi-def. Distinct from
 * gj_smoke_soft_30000 / gj_smoke_soft_29900 / gj_smoke_soft_29800 and
 * sibling 30100 milestone symbols (dyn_soft, continuum_ready, …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (30091–30100): pairs with static freestanding
 * soft smoke hosts (cgj_smoke / cgj_soft_milestone_30100). Soft rule:
 * missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=30100 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 30096. */
const char __libcgj_batch30096_marker[] = "libcgj-batch30096";

/* Smoke soft ready lamp for wave 30100 (static soft surface present). */
#define B30096_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b30096_soft(void)
{
	return B30096_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_soft_30100 - report smoke soft lamp for wave 30100.
 *
 * Always returns 1u (smoke soft surface ready). Soft pure-data product
 * tag; does not invoke cgj_smoke or any host process. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_smoke_soft_30100(void)
{
	(void)NULL;
	return b30096_soft();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_smoke_soft_30100(void)
    __attribute__((alias("gj_smoke_soft_30100")));
