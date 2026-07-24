/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch48396: smoke soft lamp (wave 48400).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_soft_48400(void);
 *     - Returns 1 (smoke soft surface ready for the milestone 48400
 *       continuum). Soft compile-time product tag; not a live smoke run.
 *   uint32_t __gj_smoke_soft_48400  (alias)
 *   __libcgj_batch48396_marker = "libcgj-batch48396"
 *
 * Milestone 48400 exclusive continuum CREATE-ONLY (48391-48400). Unique
 * gj_smoke_soft_48400 surface only; no multi-def. Distinct from
 * gj_smoke_soft_48300 / gj_smoke_soft_48200 / gj_smoke_soft_48100 and
 * sibling 48400 milestone symbols (dyn_soft, continuum_ready, …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (48391–48400): pairs with static freestanding
 * soft smoke hosts (cgj_smoke / cgj_soft_milestone_48400). Soft rule:
 * missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=48400 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 48396. */
const char __libcgj_batch48396_marker[] = "libcgj-batch48396";

/* Smoke soft ready lamp for wave 48400 (static soft surface present). */
#define B48396_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b48396_soft(void)
{
	return B48396_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_soft_48400 - report smoke soft lamp for wave 48400.
 *
 * Always returns 1u (smoke soft surface ready). Soft pure-data product
 * tag; does not invoke cgj_smoke or any host process. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_smoke_soft_48400(void)
{
	(void)NULL;
	return b48396_soft();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_smoke_soft_48400(void)
    __attribute__((alias("gj_smoke_soft_48400")));
