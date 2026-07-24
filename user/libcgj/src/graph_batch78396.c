/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch78396: smoke soft lamp (wave 78400).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_soft_78400(void);
 *     - Returns 1 (smoke soft surface ready for the milestone 78400
 *       continuum). Soft compile-time product tag; not a live smoke run.
 *   uint32_t __gj_smoke_soft_78400  (alias)
 *   __libcgj_batch78396_marker = "libcgj-batch78396"
 *
 * Milestone 78400 exclusive continuum CREATE-ONLY (25891-78400). Unique
 * gj_smoke_soft_78400 surface only; no multi-def. Distinct from
 * gj_smoke_soft_25800 / gj_smoke_soft_25700 / gj_smoke_soft_25600 and
 * sibling 78400 milestone symbols (dyn_soft, continuum_ready, …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (25891–78400): pairs with static freestanding
 * soft smoke hosts (cgj_smoke / cgj_soft_milestone_78400). Soft rule:
 * missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=78400 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 78396. */
const char __libcgj_batch78396_marker[] = "libcgj-batch78396";

/* Smoke soft ready lamp for wave 78400 (static soft surface present). */
#define B78396_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b78396_soft(void)
{
	return B78396_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_soft_78400 - report smoke soft lamp for wave 78400.
 *
 * Always returns 1u (smoke soft surface ready). Soft pure-data product
 * tag; does not invoke cgj_smoke or any host process. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_smoke_soft_78400(void)
{
	(void)NULL;
	return b78396_soft();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_smoke_soft_78400(void)
    __attribute__((alias("gj_smoke_soft_78400")));
