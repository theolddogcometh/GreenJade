/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch21196: smoke soft lamp (wave 21200).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_soft_21200(void);
 *     - Returns 1 (smoke soft surface ready for the milestone 21200
 *       continuum). Soft compile-time product tag; not a live smoke run.
 *   uint32_t __gj_smoke_soft_21200  (alias)
 *   __libcgj_batch21196_marker = "libcgj-batch21196"
 *
 * Milestone 21200 exclusive continuum CREATE-ONLY (21191-21200). Unique
 * gj_smoke_soft_21200 surface only; no multi-def. Distinct from
 * gj_smoke_soft_21100 / gj_smoke_soft_21000 / gj_smoke_soft_20900 and
 * sibling 21200 milestone symbols (dyn_soft, continuum_ready, …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (21191–21200): pairs with static freestanding
 * soft smoke hosts (cgj_smoke / cgj_soft_milestone_21200). Soft rule:
 * missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=21200 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 21196. */
const char __libcgj_batch21196_marker[] = "libcgj-batch21196";

/* Smoke soft ready lamp for wave 21200 (static soft surface present). */
#define B21196_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b21196_soft(void)
{
	return B21196_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_soft_21200 - report smoke soft lamp for wave 21200.
 *
 * Always returns 1u (smoke soft surface ready). Soft pure-data product
 * tag; does not invoke cgj_smoke or any host process. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_smoke_soft_21200(void)
{
	(void)NULL;
	return b21196_soft();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_smoke_soft_21200(void)
    __attribute__((alias("gj_smoke_soft_21200")));
