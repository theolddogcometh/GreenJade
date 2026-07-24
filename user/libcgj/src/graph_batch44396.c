/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch44396: smoke soft lamp (wave 44400).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_soft_44400(void);
 *     - Returns 1 (smoke soft surface ready for the milestone 44400
 *       continuum). Soft compile-time product tag; not a live smoke run.
 *   uint32_t __gj_smoke_soft_44400  (alias)
 *   __libcgj_batch44396_marker = "libcgj-batch44396"
 *
 * Milestone 44400 exclusive continuum CREATE-ONLY (44391-44400). Unique
 * gj_smoke_soft_44400 surface only; no multi-def. Distinct from
 * gj_smoke_soft_44300 / gj_smoke_soft_44200 / gj_smoke_soft_44100 and
 * sibling 44400 milestone symbols (dyn_soft, continuum_ready, …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (44391–44400): pairs with static freestanding
 * soft smoke hosts (cgj_smoke / cgj_soft_milestone_44400). Soft rule:
 * missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=44400 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 44396. */
const char __libcgj_batch44396_marker[] = "libcgj-batch44396";

/* Smoke soft ready lamp for wave 44400 (static soft surface present). */
#define B44396_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b44396_soft(void)
{
	return B44396_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_soft_44400 - report smoke soft lamp for wave 44400.
 *
 * Always returns 1u (smoke soft surface ready). Soft pure-data product
 * tag; does not invoke cgj_smoke or any host process. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_smoke_soft_44400(void)
{
	(void)NULL;
	return b44396_soft();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_smoke_soft_44400(void)
    __attribute__((alias("gj_smoke_soft_44400")));
