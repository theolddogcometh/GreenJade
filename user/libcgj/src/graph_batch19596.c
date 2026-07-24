/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch19596: smoke soft lamp (wave 19600).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_soft_19600(void);
 *     - Returns 1 (smoke soft surface ready for the milestone 19600
 *       continuum). Soft compile-time product tag; not a live smoke run.
 *   uint32_t __gj_smoke_soft_19600  (alias)
 *   __libcgj_batch19596_marker = "libcgj-batch19596"
 *
 * Milestone 19600 exclusive continuum CREATE-ONLY (19591-19600). Unique
 * gj_smoke_soft_19600 surface only; no multi-def. Distinct from
 * gj_smoke_soft_19500 / gj_smoke_soft_19400 / gj_smoke_soft_19300 and
 * sibling 19600 milestone symbols (dyn_soft, continuum_ready, …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (19591–19600): pairs with static freestanding
 * soft smoke hosts (cgj_smoke / cgj_soft_milestone_19600). Soft rule:
 * missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=19600 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 19596. */
const char __libcgj_batch19596_marker[] = "libcgj-batch19596";

/* Smoke soft ready lamp for wave 19600 (static soft surface present). */
#define B19596_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b19596_soft(void)
{
	return B19596_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_soft_19600 - report smoke soft lamp for wave 19600.
 *
 * Always returns 1u (smoke soft surface ready). Soft pure-data product
 * tag; does not invoke cgj_smoke or any host process. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_smoke_soft_19600(void)
{
	(void)NULL;
	return b19596_soft();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_smoke_soft_19600(void)
    __attribute__((alias("gj_smoke_soft_19600")));
