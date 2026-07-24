/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch63396: smoke soft lamp (wave 63400).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_soft_63400(void);
 *     - Returns 1 (smoke soft surface ready for the milestone 63400
 *       continuum). Soft compile-time product tag; not a live smoke run.
 *   uint32_t __gj_smoke_soft_63400  (alias)
 *   __libcgj_batch63396_marker = "libcgj-batch63396"
 *
 * Milestone 63400 exclusive continuum CREATE-ONLY (25891-63400). Unique
 * gj_smoke_soft_63400 surface only; no multi-def. Distinct from
 * gj_smoke_soft_25800 / gj_smoke_soft_25700 / gj_smoke_soft_25600 and
 * sibling 63400 milestone symbols (dyn_soft, continuum_ready, …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (25891–63400): pairs with static freestanding
 * soft smoke hosts (cgj_smoke / cgj_soft_milestone_63400). Soft rule:
 * missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=63400 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 63396. */
const char __libcgj_batch63396_marker[] = "libcgj-batch63396";

/* Smoke soft ready lamp for wave 63400 (static soft surface present). */
#define B63396_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b63396_soft(void)
{
	return B63396_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_soft_63400 - report smoke soft lamp for wave 63400.
 *
 * Always returns 1u (smoke soft surface ready). Soft pure-data product
 * tag; does not invoke cgj_smoke or any host process. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_smoke_soft_63400(void)
{
	(void)NULL;
	return b63396_soft();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_smoke_soft_63400(void)
    __attribute__((alias("gj_smoke_soft_63400")));
