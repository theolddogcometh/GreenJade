/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch78896: smoke soft lamp (wave 78900).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_soft_78900(void);
 *     - Returns 1 (smoke soft surface ready for the milestone 78900
 *       continuum). Soft compile-time product tag; not a live smoke run.
 *   uint32_t __gj_smoke_soft_78900  (alias)
 *   __libcgj_batch78896_marker = "libcgj-batch78896"
 *
 * Milestone 78900 exclusive continuum CREATE-ONLY (25891-78900). Unique
 * gj_smoke_soft_78900 surface only; no multi-def. Distinct from
 * gj_smoke_soft_25800 / gj_smoke_soft_25700 / gj_smoke_soft_25600 and
 * sibling 78900 milestone symbols (dyn_soft, continuum_ready, …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (25891–78900): pairs with static freestanding
 * soft smoke hosts (cgj_smoke / cgj_soft_milestone_78900). Soft rule:
 * missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=78900 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 78896. */
const char __libcgj_batch78896_marker[] = "libcgj-batch78896";

/* Smoke soft ready lamp for wave 78900 (static soft surface present). */
#define B78896_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b78896_soft(void)
{
	return B78896_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_soft_78900 - report smoke soft lamp for wave 78900.
 *
 * Always returns 1u (smoke soft surface ready). Soft pure-data product
 * tag; does not invoke cgj_smoke or any host process. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_smoke_soft_78900(void)
{
	(void)NULL;
	return b78896_soft();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_smoke_soft_78900(void)
    __attribute__((alias("gj_smoke_soft_78900")));
