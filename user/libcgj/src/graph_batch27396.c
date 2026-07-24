/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch27396: smoke soft lamp (wave 27400).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_soft_27400(void);
 *     - Returns 1 (smoke soft surface ready for the milestone 27400
 *       continuum). Soft compile-time product tag; not a live smoke run.
 *   uint32_t __gj_smoke_soft_27400  (alias)
 *   __libcgj_batch27396_marker = "libcgj-batch27396"
 *
 * Milestone 27400 exclusive continuum CREATE-ONLY (27391-27400). Unique
 * gj_smoke_soft_27400 surface only; no multi-def. Distinct from
 * gj_smoke_soft_27300 / gj_smoke_soft_27200 / gj_smoke_soft_27100 and
 * sibling 27400 milestone symbols (dyn_soft, continuum_ready, …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (27391–27400): pairs with static freestanding
 * soft smoke hosts (cgj_smoke / cgj_soft_milestone_27400). Soft rule:
 * missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=27400 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 27396. */
const char __libcgj_batch27396_marker[] = "libcgj-batch27396";

/* Smoke soft ready lamp for wave 27400 (static soft surface present). */
#define B27396_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b27396_soft(void)
{
	return B27396_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_soft_27400 - report smoke soft lamp for wave 27400.
 *
 * Always returns 1u (smoke soft surface ready). Soft pure-data product
 * tag; does not invoke cgj_smoke or any host process. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_smoke_soft_27400(void)
{
	(void)NULL;
	return b27396_soft();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_smoke_soft_27400(void)
    __attribute__((alias("gj_smoke_soft_27400")));
