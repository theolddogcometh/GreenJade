/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch18196: smoke soft lamp (wave 18200).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_soft_18200(void);
 *     - Returns 1 (smoke soft surface ready for the milestone 18200
 *       continuum). Soft compile-time product tag; not a live smoke run.
 *   uint32_t __gj_smoke_soft_18200  (alias)
 *   __libcgj_batch18196_marker = "libcgj-batch18196"
 *
 * Milestone 18200 exclusive continuum CREATE-ONLY (18191-18200). Unique
 * gj_smoke_soft_18200 surface only; no multi-def. Distinct from
 * gj_smoke_soft_18000 / gj_smoke_soft_17900 / gj_smoke_soft_17800 and
 * sibling 18200 milestone symbols (dyn_soft, continuum_ready, …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (18191–18200): pairs with static freestanding
 * soft smoke hosts (cgj_smoke / cgj_soft_milestone_18200). Soft rule:
 * missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=18200 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 18196. */
const char __libcgj_batch18196_marker[] = "libcgj-batch18196";

/* Smoke soft ready lamp for wave 18200 (static soft surface present). */
#define B18196_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b18196_soft(void)
{
	return B18196_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_soft_18200 - report smoke soft lamp for wave 18200.
 *
 * Always returns 1u (smoke soft surface ready). Soft pure-data product
 * tag; does not invoke cgj_smoke or any host process. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_smoke_soft_18200(void)
{
	(void)NULL;
	return b18196_soft();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_smoke_soft_18200(void)
    __attribute__((alias("gj_smoke_soft_18200")));
