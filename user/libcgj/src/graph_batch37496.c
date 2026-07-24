/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch37496: smoke soft lamp (wave 37500).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_soft_37500(void);
 *     - Returns 1 (smoke soft surface ready for the milestone 37500
 *       continuum). Soft compile-time product tag; not a live smoke run.
 *   uint32_t __gj_smoke_soft_37500  (alias)
 *   __libcgj_batch37496_marker = "libcgj-batch37496"
 *
 * Milestone 37500 exclusive continuum CREATE-ONLY (37491-37500). Unique
 * gj_smoke_soft_37500 surface only; no multi-def. Distinct from
 * gj_smoke_soft_37400 / gj_smoke_soft_37300 / gj_smoke_soft_37200 and
 * sibling 37500 milestone symbols (dyn_soft, continuum_ready, …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (37491–37500): pairs with static freestanding
 * soft smoke hosts (cgj_smoke / cgj_soft_milestone_37500). Soft rule:
 * missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=37500 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 37496. */
const char __libcgj_batch37496_marker[] = "libcgj-batch37496";

/* Smoke soft ready lamp for wave 37500 (static soft surface present). */
#define B37496_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b37496_soft(void)
{
	return B37496_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_soft_37500 - report smoke soft lamp for wave 37500.
 *
 * Always returns 1u (smoke soft surface ready). Soft pure-data product
 * tag; does not invoke cgj_smoke or any host process. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_smoke_soft_37500(void)
{
	(void)NULL;
	return b37496_soft();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_smoke_soft_37500(void)
    __attribute__((alias("gj_smoke_soft_37500")));
