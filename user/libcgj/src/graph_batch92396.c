/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch92396: smoke soft lamp (wave 92400).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_soft_92400(void);
 *     - Returns 1 (smoke soft surface ready for the milestone 92400
 *       continuum). Soft compile-time product tag; not a live smoke run.
 *   uint32_t __gj_smoke_soft_92400  (alias)
 *   __libcgj_batch92396_marker = "libcgj-batch92396"
 *
 * Milestone 92400 exclusive continuum CREATE-ONLY (25891-92400). Unique
 * gj_smoke_soft_92400 surface only; no multi-def. Distinct from
 * gj_smoke_soft_25800 / gj_smoke_soft_25700 / gj_smoke_soft_25600 and
 * sibling 92400 milestone symbols (dyn_soft, continuum_ready, …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (25891–92400): pairs with static freestanding
 * soft smoke hosts (cgj_smoke / cgj_soft_milestone_92400). Soft rule:
 * missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=92400 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 92396. */
const char __libcgj_batch92396_marker[] = "libcgj-batch92396";

/* Smoke soft ready lamp for wave 92400 (static soft surface present). */
#define B92396_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b92396_soft(void)
{
	return B92396_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_soft_92400 - report smoke soft lamp for wave 92400.
 *
 * Always returns 1u (smoke soft surface ready). Soft pure-data product
 * tag; does not invoke cgj_smoke or any host process. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_smoke_soft_92400(void)
{
	(void)NULL;
	return b92396_soft();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_smoke_soft_92400(void)
    __attribute__((alias("gj_smoke_soft_92400")));
