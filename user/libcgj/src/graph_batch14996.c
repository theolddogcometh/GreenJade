/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14996: smoke soft lamp (wave 15000).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_soft_15000(void);
 *     - Returns 1 (smoke soft surface ready for the milestone 15000
 *       continuum). Soft compile-time product tag; not a live smoke run.
 *   uint32_t __gj_smoke_soft_15000  (alias)
 *   __libcgj_batch14996_marker = "libcgj-batch14996"
 *
 * Milestone 15000 exclusive continuum CREATE-ONLY (14991-15000). Unique
 * gj_smoke_soft_15000 surface only; no multi-def. Distinct from
 * gj_smoke_soft_14900 / gj_smoke_soft_14800 / gj_smoke_soft_14700 and
 * sibling 15000 milestone symbols (dyn_soft, continuum_ready, …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (14991–15000): pairs with static freestanding
 * soft smoke hosts (cgj_smoke / cgj_soft_milestone_15000). Soft rule:
 * missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=15000 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 14996. */
const char __libcgj_batch14996_marker[] = "libcgj-batch14996";

/* Smoke soft ready lamp for wave 15000 (static soft surface present). */
#define B14996_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14996_soft(void)
{
	return B14996_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_soft_15000 - report smoke soft lamp for wave 15000.
 *
 * Always returns 1u (smoke soft surface ready). Soft pure-data product
 * tag; does not invoke cgj_smoke or any host process. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_smoke_soft_15000(void)
{
	(void)NULL;
	return b14996_soft();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_smoke_soft_15000(void)
    __attribute__((alias("gj_smoke_soft_15000")));
