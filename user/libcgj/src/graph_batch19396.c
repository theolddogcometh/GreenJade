/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch19396: smoke soft lamp (wave 19400).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_soft_19400(void);
 *     - Returns 1 (smoke soft surface ready for the milestone 19400
 *       continuum). Soft compile-time product tag; not a live smoke run.
 *   uint32_t __gj_smoke_soft_19400  (alias)
 *   __libcgj_batch19396_marker = "libcgj-batch19396"
 *
 * Milestone 19400 exclusive continuum CREATE-ONLY (19391-19400). Unique
 * gj_smoke_soft_19400 surface only; no multi-def. Distinct from
 * gj_smoke_soft_19300 / gj_smoke_soft_19200 / gj_smoke_soft_19100 and
 * sibling 19400 milestone symbols (dyn_soft, continuum_ready, …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (19391–19400): pairs with static freestanding
 * soft smoke hosts (cgj_smoke / cgj_soft_milestone_19400). Soft rule:
 * missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=19400 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 19396. */
const char __libcgj_batch19396_marker[] = "libcgj-batch19396";

/* Smoke soft ready lamp for wave 19400 (static soft surface present). */
#define B19396_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b19396_soft(void)
{
	return B19396_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_soft_19400 - report smoke soft lamp for wave 19400.
 *
 * Always returns 1u (smoke soft surface ready). Soft pure-data product
 * tag; does not invoke cgj_smoke or any host process. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_smoke_soft_19400(void)
{
	(void)NULL;
	return b19396_soft();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_smoke_soft_19400(void)
    __attribute__((alias("gj_smoke_soft_19400")));
