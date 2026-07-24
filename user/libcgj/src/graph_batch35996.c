/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch35996: smoke soft lamp (wave 36000).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_soft_36000(void);
 *     - Returns 1 (smoke soft surface ready for the milestone 36000
 *       continuum). Soft compile-time product tag; not a live smoke run.
 *   uint32_t __gj_smoke_soft_36000  (alias)
 *   __libcgj_batch35996_marker = "libcgj-batch35996"
 *
 * Milestone 36000 exclusive continuum CREATE-ONLY (35991-36000). Unique
 * gj_smoke_soft_36000 surface only; no multi-def. Distinct from
 * gj_smoke_soft_35900 / gj_smoke_soft_35800 / gj_smoke_soft_35700 and
 * sibling 36000 milestone symbols (dyn_soft, continuum_ready, …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (35991–36000): pairs with static freestanding
 * soft smoke hosts (cgj_smoke / cgj_soft_milestone_36000). Soft rule:
 * missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=36000 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 35996. */
const char __libcgj_batch35996_marker[] = "libcgj-batch35996";

/* Smoke soft ready lamp for wave 36000 (static soft surface present). */
#define B35996_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b35996_soft(void)
{
	return B35996_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_soft_36000 - report smoke soft lamp for wave 36000.
 *
 * Always returns 1u (smoke soft surface ready). Soft pure-data product
 * tag; does not invoke cgj_smoke or any host process. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_smoke_soft_36000(void)
{
	(void)NULL;
	return b35996_soft();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_smoke_soft_36000(void)
    __attribute__((alias("gj_smoke_soft_36000")));
