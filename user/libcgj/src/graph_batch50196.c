/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch50196: smoke soft lamp (wave 50200).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_soft_50200(void);
 *     - Returns 1 (smoke soft surface ready for the milestone 50200
 *       continuum). Soft compile-time product tag; not a live smoke run.
 *   uint32_t __gj_smoke_soft_50200  (alias)
 *   __libcgj_batch50196_marker = "libcgj-batch50196"
 *
 * Milestone 50200 exclusive continuum CREATE-ONLY (50191-50200). Unique
 * gj_smoke_soft_50200 surface only; no multi-def. Distinct from
 * gj_smoke_soft_50100 / gj_smoke_soft_50200 / gj_smoke_soft_49900 and
 * sibling 50200 milestone symbols (dyn_soft, continuum_ready, …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (50191–50200): pairs with static freestanding
 * soft smoke hosts (cgj_smoke / cgj_soft_milestone_50200). Soft rule:
 * missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=50200 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 50196. */
const char __libcgj_batch50196_marker[] = "libcgj-batch50196";

/* Smoke soft ready lamp for wave 50200 (static soft surface present). */
#define B50196_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b50196_soft(void)
{
	return B50196_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_soft_50200 - report smoke soft lamp for wave 50200.
 *
 * Always returns 1u (smoke soft surface ready). Soft pure-data product
 * tag; does not invoke cgj_smoke or any host process. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_smoke_soft_50200(void)
{
	(void)NULL;
	return b50196_soft();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_smoke_soft_50200(void)
    __attribute__((alias("gj_smoke_soft_50200")));
