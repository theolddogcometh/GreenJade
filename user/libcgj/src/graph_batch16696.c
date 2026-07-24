/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch16696: smoke soft lamp (wave 16700).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_soft_16700(void);
 *     - Returns 1 (smoke soft surface ready for the milestone 16700
 *       continuum). Soft compile-time product tag; not a live smoke run.
 *   uint32_t __gj_smoke_soft_16700  (alias)
 *   __libcgj_batch16696_marker = "libcgj-batch16696"
 *
 * Milestone 16700 exclusive continuum CREATE-ONLY (16691-16700). Unique
 * gj_smoke_soft_16700 surface only; no multi-def. Distinct from
 * gj_smoke_soft_16600 / gj_smoke_soft_16500 / gj_smoke_soft_16400 and
 * sibling 16700 milestone symbols (dyn_soft, continuum_ready, …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (16691–16700): pairs with static freestanding
 * soft smoke hosts (cgj_smoke / cgj_soft_milestone_16700). Soft rule:
 * missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=16700 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 16696. */
const char __libcgj_batch16696_marker[] = "libcgj-batch16696";

/* Smoke soft ready lamp for wave 16700 (static soft surface present). */
#define B16696_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b16696_soft(void)
{
	return B16696_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_soft_16700 - report smoke soft lamp for wave 16700.
 *
 * Always returns 1u (smoke soft surface ready). Soft pure-data product
 * tag; does not invoke cgj_smoke or any host process. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_smoke_soft_16700(void)
{
	(void)NULL;
	return b16696_soft();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_smoke_soft_16700(void)
    __attribute__((alias("gj_smoke_soft_16700")));
