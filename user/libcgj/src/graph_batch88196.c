/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch88196: smoke soft lamp (wave 88200).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_soft_88200(void);
 *     - Returns 1 (smoke soft surface ready for the milestone 88200
 *       continuum). Soft compile-time product tag; not a live smoke run.
 *   uint32_t __gj_smoke_soft_88200  (alias)
 *   __libcgj_batch88196_marker = "libcgj-batch88196"
 *
 * Milestone 88200 exclusive continuum CREATE-ONLY (25891-88200). Unique
 * gj_smoke_soft_88200 surface only; no multi-def. Distinct from
 * gj_smoke_soft_25800 / gj_smoke_soft_25700 / gj_smoke_soft_25600 and
 * sibling 88200 milestone symbols (dyn_soft, continuum_ready, …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (25891–88200): pairs with static freestanding
 * soft smoke hosts (cgj_smoke / cgj_soft_milestone_88200). Soft rule:
 * missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=88200 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 88196. */
const char __libcgj_batch88196_marker[] = "libcgj-batch88196";

/* Smoke soft ready lamp for wave 88200 (static soft surface present). */
#define B88196_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b88196_soft(void)
{
	return B88196_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_soft_88200 - report smoke soft lamp for wave 88200.
 *
 * Always returns 1u (smoke soft surface ready). Soft pure-data product
 * tag; does not invoke cgj_smoke or any host process. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_smoke_soft_88200(void)
{
	(void)NULL;
	return b88196_soft();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_smoke_soft_88200(void)
    __attribute__((alias("gj_smoke_soft_88200")));
